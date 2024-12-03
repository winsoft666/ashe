#include "ashe/win/firewall.h"
#include <assert.h>
#include <objbase.h>
#include <oleauto.h>

namespace ashe {
namespace win {
HRESULT Firewall::initialize() {
    HRESULT hr = S_OK;
    INetFwMgr* fwMgr = NULL;
    INetFwPolicy* fwPolicy = NULL;

    do {
        // Create an instance of the firewall settings manager.
        hr = CoCreateInstance(
            __uuidof(NetFwMgr),
            NULL,
            CLSCTX_INPROC_SERVER,
            __uuidof(INetFwMgr),
            (void**)&fwMgr);
        if (FAILED(hr)) {
            break;
        }

        // Retrieve the local firewall policy.
        hr = fwMgr->get_LocalPolicy(&fwPolicy);
        if (FAILED(hr)) {
            break;
        }

        // Retrieve the firewall profile currently in effect.
        hr = fwPolicy->get_CurrentProfile(&fwProfile_);
        if (FAILED(hr)) {
            break;
        }

    } while (false);

    // Release the local firewall policy.
    if (fwPolicy != NULL) {
        fwPolicy->Release();
    }

    // Release the firewall settings manager.
    if (fwMgr != NULL) {
        fwMgr->Release();
    }

    return hr;
}

void Firewall::cleanup() {
    // Release the firewall profile.
    if (fwProfile_ != NULL) {
        fwProfile_->Release();
    }
}

HRESULT Firewall::isOn(OUT bool* fwOn) {
    HRESULT hr = S_OK;
    VARIANT_BOOL fwEnabled;

    assert(fwProfile_ != NULL);
    assert(fwOn != NULL);

    if (!fwProfile_)
        return E_FAIL;

    *fwOn = false;

    do {
        // Get the current state of the firewall.
        hr = fwProfile_->get_FirewallEnabled(&fwEnabled);
        if (FAILED(hr)) {
            break;
        }

        // Check to see if the firewall is on.
        if (fwEnabled != VARIANT_FALSE) {
            *fwOn = TRUE;
        }
    } while (false);

    return hr;
}

HRESULT Firewall::turnOn() {
    HRESULT hr = S_OK;
    bool fwOn = false;

    assert(fwProfile_ != NULL);
    if (!fwProfile_)
        return E_FAIL;

    do {
        // Check to see if the firewall is off.
        hr = isOn(&fwOn);
        if (FAILED(hr)) {
            break;
        }

        // If it is, turn it on.
        if (!fwOn) {
            // Turn the firewall on.
            hr = fwProfile_->put_FirewallEnabled(VARIANT_TRUE);
            if (FAILED(hr)) {
                break;
            }
        }
    } while (false);

    return hr;
}

HRESULT Firewall::turnOff() {
    HRESULT hr = S_OK;
    bool fwOn = false;

    assert(fwProfile_ != NULL);
    if (!fwProfile_)
        return E_FAIL;

    do {
        // Check to see if the firewall is on.
        hr = isOn(&fwOn);
        if (FAILED(hr)) {
            break;
        }

        // If it is, turn it off.
        if (fwOn) {
            // Turn the firewall off.
            hr = fwProfile_->put_FirewallEnabled(VARIANT_FALSE);
            if (FAILED(hr)) {
                break;
            }
        }
    } while (false);

    return hr;
}

HRESULT Firewall::isAppEnabled(IN const wchar_t* fwProcessImageFileName, OUT bool* fwAppEnabled) {
    HRESULT hr = S_OK;
    BSTR fwBstrProcessImageFileName = NULL;
    VARIANT_BOOL fwEnabled;
    INetFwAuthorizedApplication* fwApp = NULL;
    INetFwAuthorizedApplications* fwApps = NULL;

    assert(fwProfile_ != NULL);
    assert(fwProcessImageFileName != NULL);
    assert(fwAppEnabled != NULL);

    if (!fwProfile_)
        return E_FAIL;

    *fwAppEnabled = false;

    do {
        // Retrieve the authorized application collection.
        hr = fwProfile_->get_AuthorizedApplications(&fwApps);
        if (FAILED(hr)) {
            break;
        }

        // Allocate a BSTR for the process image file name.
        fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
        if (fwBstrProcessImageFileName == NULL) {
            hr = E_OUTOFMEMORY;
            break;
        }

        // Attempt to retrieve the authorized application.
        hr = fwApps->Item(fwBstrProcessImageFileName, &fwApp);
        if (SUCCEEDED(hr)) {
            // Find out if the authorized application is enabled.
            hr = fwApp->get_Enabled(&fwEnabled);
            if (FAILED(hr)) {
                break;
            }

            if (fwEnabled != VARIANT_FALSE) {
                // The authorized application is enabled.
                *fwAppEnabled = TRUE;
            }
        }
        else {
            // The authorized application was not in the collection.
            hr = S_OK;
        }
    } while (false);

    // Free the BSTR.
    SysFreeString(fwBstrProcessImageFileName);

    // Release the authorized application instance.
    if (fwApp != NULL) {
        fwApp->Release();
    }

    // Release the authorized application collection.
    if (fwApps != NULL) {
        fwApps->Release();
    }

    return hr;
}

HRESULT Firewall::addApp(
    IN const wchar_t* fwProcessImageFileName,
    IN const wchar_t* fwName) {
    HRESULT hr = S_OK;
    bool fwAppEnabled = false;
    BSTR fwBstrName = NULL;
    BSTR fwBstrProcessImageFileName = NULL;
    INetFwAuthorizedApplication* fwApp = NULL;
    INetFwAuthorizedApplications* fwApps = NULL;

    assert(fwProfile_ != NULL);
    assert(fwProcessImageFileName != NULL);
    assert(fwName != NULL);
    if (!fwProfile_)
        return E_FAIL;

    do {
        // First check to see if the application is already authorized.
        hr = isAppEnabled(
            fwProcessImageFileName,
            &fwAppEnabled);
        if (FAILED(hr)) {
            break;
        }

        // Only add the application if it isn't already authorized.
        if (!fwAppEnabled) {
            // Retrieve the authorized application collection.
            hr = fwProfile_->get_AuthorizedApplications(&fwApps);
            if (FAILED(hr)) {
                break;
            }

            // Create an instance of an authorized application.
            hr = CoCreateInstance(
                __uuidof(NetFwAuthorizedApplication),
                NULL,
                CLSCTX_INPROC_SERVER,
                __uuidof(INetFwAuthorizedApplication),
                (void**)&fwApp);
            if (FAILED(hr)) {
                break;
            }

            // Allocate a BSTR for the process image file name.
            fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
            if (fwBstrProcessImageFileName == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }

            // Set the process image file name.
            hr = fwApp->put_ProcessImageFileName(fwBstrProcessImageFileName);
            if (FAILED(hr)) {
                break;
            }

            // Allocate a BSTR for the application friendly name.
            fwBstrName = SysAllocString(fwName);
            if (SysStringLen(fwBstrName) == 0) {
                hr = E_OUTOFMEMORY;
                break;
            }

            // Set the application friendly name.
            hr = fwApp->put_Name(fwBstrName);
            if (FAILED(hr)) {
                break;
            }

            // Add the application to the collection.
            hr = fwApps->Add(fwApp);
            if (FAILED(hr)) {
                break;
            }
        }
    } while (false);

    // Free the BSTRs.
    SysFreeString(fwBstrName);
    SysFreeString(fwBstrProcessImageFileName);

    // Release the authorized application instance.
    if (fwApp != NULL) {
        fwApp->Release();
    }

    // Release the authorized application collection.
    if (fwApps != NULL) {
        fwApps->Release();
    }

    return hr;
}

HRESULT Firewall::isPortEnabled(
    IN long portNumber,
    IN NET_FW_IP_PROTOCOL ipProtocol,
    OUT bool* fwPortEnabled) {
    HRESULT hr = S_OK;
    VARIANT_BOOL fwEnabled;
    INetFwOpenPort* fwOpenPort = NULL;
    INetFwOpenPorts* fwOpenPorts = NULL;

    assert(fwProfile_ != NULL);
    assert(fwPortEnabled != NULL);
    if (!fwProfile_)
        return E_FAIL;

    *fwPortEnabled = false;

    do {
        // Retrieve the globally open ports collection.
        hr = fwProfile_->get_GloballyOpenPorts(&fwOpenPorts);
        if (FAILED(hr)) {
            break;
        }

        // Attempt to retrieve the globally open port.
        hr = fwOpenPorts->Item(portNumber, ipProtocol, &fwOpenPort);
        if (SUCCEEDED(hr)) {
            // Find out if the globally open port is enabled.
            hr = fwOpenPort->get_Enabled(&fwEnabled);
            if (FAILED(hr)) {
                break;
            }

            if (fwEnabled != VARIANT_FALSE) {
                // The globally open port is enabled.
                *fwPortEnabled = true;
            }
        }
        else {
            // The globally open port was not in the collection.
            hr = S_OK;
        }
    } while (false);

    // Release the globally open port.
    if (fwOpenPort != NULL) {
        fwOpenPort->Release();
    }

    // Release the globally open ports collection.
    if (fwOpenPorts != NULL) {
        fwOpenPorts->Release();
    }

    return hr;
}

HRESULT Firewall::addPort(
    IN long portNumber,
    IN NET_FW_IP_PROTOCOL ipProtocol,
    IN const wchar_t* name) {
    HRESULT hr = S_OK;
    bool fwPortEnabled = false;
    BSTR fwBstrName = NULL;
    INetFwOpenPort* fwOpenPort = NULL;
    INetFwOpenPorts* fwOpenPorts = NULL;

    assert(fwProfile_ != NULL);
    assert(name != NULL);
    if (!fwProfile_)
        return E_FAIL;

    do {
        // First check to see if the port is already added.
        hr = isPortEnabled(
            portNumber,
            ipProtocol,
            &fwPortEnabled);
        if (FAILED(hr)) {
            break;
        }

        // Only add the port if it isn't already added.
        if (!fwPortEnabled) {
            // Retrieve the collection of globally open ports.
            hr = fwProfile_->get_GloballyOpenPorts(&fwOpenPorts);
            if (FAILED(hr)) {
                break;
            }

            // Create an instance of an open port.
            hr = CoCreateInstance(
                __uuidof(NetFwOpenPort),
                NULL,
                CLSCTX_INPROC_SERVER,
                __uuidof(INetFwOpenPort),
                (void**)&fwOpenPort);
            if (FAILED(hr)) {
                break;
            }

            // Set the port number.
            hr = fwOpenPort->put_Port(portNumber);
            if (FAILED(hr)) {
                break;
            }

            // Set the IP protocol.
            hr = fwOpenPort->put_Protocol(ipProtocol);
            if (FAILED(hr)) {
                break;
            }

            // Allocate a BSTR for the friendly name of the port.
            fwBstrName = SysAllocString(name);
            if (SysStringLen(fwBstrName) == 0) {
                hr = E_OUTOFMEMORY;
                break;
            }

            // Set the friendly name of the port.
            hr = fwOpenPort->put_Name(fwBstrName);
            if (FAILED(hr)) {
                break;
            }

            // Opens the port and adds it to the collection.
            hr = fwOpenPorts->Add(fwOpenPort);
            if (FAILED(hr)) {
                break;
            }
        }
    } while (false);

    // Free the BSTR.
    SysFreeString(fwBstrName);

    // Release the open port instance.
    if (fwOpenPort != NULL) {
        fwOpenPort->Release();
    }

    // Release the globally open ports collection.
    if (fwOpenPorts != NULL) {
        fwOpenPorts->Release();
    }

    return hr;
}
}  // namespace win
}  // namespace ashe
