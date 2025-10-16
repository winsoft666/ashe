/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2020~2024 winsoft666 <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef ASHE_WIN_SIGNATURE_H_
#define ASHE_WIN_SIGNATURE_H_
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#include <string>

namespace ashe {
namespace win {
// Windows 下的文件数字签名的校验及相关信息获取
//
class ASHE_API Signature {
   public:
    enum class SignVerifyResult {
        UnknownError = 0,
        Verified = 1,  // 签名合法
        NotSigned = 2,
        SubjectNotTrusted = 3,
        CertificateNotTrusted = 4,
        VerifyFailedDueToSecurityOption = 5,
        Expired = 6,
    };

    Signature(const wchar_t* pFilePath);

    // 校验签名是否合法
    SignVerifyResult verify();

    // 获取签名者姓名
    std::wstring getSigner();

   protected:
    std::wstring filePath_;
};
}  // namespace win
}  // namespace ashe
#endif  // !ASHE_WIN_SIGNATURE_H_
