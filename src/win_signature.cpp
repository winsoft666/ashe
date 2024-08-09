#include "ashe/win_signature.h"

#ifdef ASHE_WIN
#include "ashe/windows_lite.h"
#include <Softpub.h>
#include <wincrypt.h>
#include <wintrust.h>
#include "ashe/check_failure.h"

namespace ashe {

WinSignature::WinSignature(const wchar_t* pFilePath) :
    filePath_(pFilePath) {
}

WinSignature::SignVerifyResult WinSignature::verify() {
    SignVerifyResult result = SignVerifyResult::UnknownError;
    LONG lStatus;
    DWORD dwLastError;

    if (filePath_.empty())
        return result;

    // Initialize the WINTRUST_FILE_INFO structure.
    WINTRUST_FILE_INFO FileData;
    memset(&FileData, 0, sizeof(FileData));
    FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
    FileData.pcwszFilePath = filePath_.c_str();

    GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    WINTRUST_DATA WinTrustData;
    memset(&WinTrustData, 0, sizeof(WinTrustData));
    WinTrustData.cbStruct = sizeof(WinTrustData);
    WinTrustData.dwUIChoice = WTD_UI_NONE;
    WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
    WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
    WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY;
    WinTrustData.pFile = &FileData;

    // WinVerifyTrust verifies signatures as specified by the GUID and Wintrust_Data.
    lStatus = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);

    switch (lStatus) {
        case ERROR_SUCCESS:
            /*
            Signed file:
                - Hash that represents the subject is trusted.

                - Trusted publisher without any verification errors.

                - UI was disabled in dwUIChoice. No publisher or 
                    time stamp chain errors.

                - UI was enabled in dwUIChoice and the user clicked 
                    "Yes" when asked to install and run the signed 
                    subject.
            */
            result = SignVerifyResult::Verified;
            break;

        case TRUST_E_NOSIGNATURE:
            // The file was not signed or had a signature that was not valid.
            // Get the reason for no signature.
            dwLastError = GetLastError();
            if (TRUST_E_NOSIGNATURE == dwLastError || TRUST_E_SUBJECT_FORM_UNKNOWN == dwLastError || TRUST_E_PROVIDER_UNKNOWN == dwLastError) {
                result = SignVerifyResult::NotSigned;
            }
            else {
                // The signature was not valid or there was an error opening the file.
                result = SignVerifyResult::UnknownError;
            }

            break;

        case TRUST_E_EXPLICIT_DISTRUST:
            // The hash that represents the subject or the publisher is not allowed by the admin or user.
            result = SignVerifyResult::CertificateNotTrusted;
            break;

        case TRUST_E_SUBJECT_NOT_TRUSTED:
            // The user clicked "No" when asked to install and run.
            result = SignVerifyResult::SubjectNotTrusted;
            break;

        case CRYPT_E_SECURITY_SETTINGS:
            result = SignVerifyResult::VerifyFailedDueToSecurityOption;
            break;

        default:
            ASHE_CHECK_FAILURE_STATUS(lStatus, L"WinVerifyTrust failed");
            break;
    }

    // Any hWVTStateData must be released by a call with close.
    WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;

    lStatus = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);

    return result;
}

std::wstring WinSignature::getSigner() {
    std::wstring signer;

    if (filePath_.empty())
        return signer;

    WINTRUST_FILE_INFO FileData;
    memset(&FileData, 0, sizeof(FileData));
    FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
    FileData.pcwszFilePath = filePath_.c_str();

    GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    WINTRUST_DATA WinTrustData;
    memset(&WinTrustData, 0, sizeof(WinTrustData));
    WinTrustData.cbStruct = sizeof(WINTRUST_DATA);
    WinTrustData.dwUIChoice = WTD_UI_NONE;
    WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
    WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
    WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY;
    WinTrustData.pFile = &FileData;

    LONG lStatus = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);

    if (lStatus == ERROR_SUCCESS) {
        CRYPT_PROVIDER_DATA* pProvData = WTHelperProvDataFromStateData(WinTrustData.hWVTStateData);
        if (pProvData) {
            CRYPT_PROVIDER_SGNR* pProvSigner = WTHelperGetProvSignerFromChain(pProvData, 0, FALSE, 0);
            if (pProvSigner) {
                PCCERT_CONTEXT pCertContext = pProvSigner->pasCertChain->pCert;
                if (pCertContext) {
                    wchar_t szName[512] = {0};
                    if (CertGetNameStringW(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, szName, 512) > 1) {
                        signer = szName;
                    }
                    CertFreeCertificateContext(pCertContext);
                }
            }
        }
    }

    WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);

    return signer;
}

}  // namespace ashe

#endif  // ASHE_WIN