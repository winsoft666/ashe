#ifndef WIN_FIREWALL_HPP_
#define WIN_FIREWALL_HPP_
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#ifdef ASHE_WIN
#include "ashe/windows_lite.h"
#include <netfw.h>

namespace ashe {
class Firewall {
   public:
    HRESULT initialize();
    void cleanup();

    HRESULT isOn(OUT bool* fwOn);
    HRESULT turnOn();
    HRESULT turnOff();

    HRESULT isAppEnabled(IN const wchar_t* fwProcessImageFileName, OUT bool* fwAppEnabled);
    HRESULT addApp(IN const wchar_t* fwProcessImageFileName, IN const wchar_t* fwName);

    HRESULT isPortEnabled(IN long portNumber, IN NET_FW_IP_PROTOCOL ipProtocol, OUT bool* fwPortEnabled);
    HRESULT addPort(IN long portNumber, IN NET_FW_IP_PROTOCOL ipProtocol, IN const wchar_t* name);

   private:
    INetFwProfile* fwProfile_ = NULL;
};
}  // namespace ashe
#endif  // ASHE_WIN
#endif  // !WIN_FIREWALL_HPP_
