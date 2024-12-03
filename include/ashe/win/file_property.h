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

#ifndef ASHE_WIN_FILE_INFO_H__
#define ASHE_WIN_FILE_INFO_H__
#pragma once

#include "ashe/arch.h"
#include "ashe/config.h"
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#include <string>
#include "ashe/time_util.h"

namespace ashe {
namespace win {
class ASHE_API FileProperty {
   public:
    FileProperty();
    ~FileProperty() = default;

    bool open(HMODULE hModule = NULL);
    bool open(const std::wstring& strFileName);

   public:
    WORD getFileVersion(int nIndex) const;
    WORD getProductVersion(int nIndex) const;
    DWORD getFileFlagsMask() const;
    DWORD getFileFlags() const;
    DWORD getFileOs() const;
    DWORD getFileType() const;
    DWORD getFileSubtype() const;

    std::wstring getCompanyName();
    std::wstring getFileDescription();
    std::wstring getFileVersion();
    std::wstring getFileVersionEx();
    std::wstring getInternalName();
    std::wstring getLegalCopyright();
    std::wstring getOriginalFileName();
    std::wstring getProductName();
    std::wstring getProductVersion();
    std::wstring getComments();
    std::wstring getLegalTrademarks();
    std::wstring getPrivateBuild();
    std::wstring getSpecialBuild();

    // Windows的文件时间为一个64位整数（用FILETIME结构体存储）,它记录从1601-1-1
    // 00:00:00到当前格林威治时间（UTC）所经过的100纳秒(ns)数 See: https://blog.csdn.net/china_jeffery/article/details/78409614
    //
    FILETIME getCreationTime() const;
    FILETIME getLastAccessTime() const;
    FILETIME getLastWriteTime() const;

   private:
    virtual void reset();
    bool getTranslationId(LPVOID lpData,
                          UINT unBlockSize,
                          WORD wLangId,
                          DWORD& dwId,
                          BOOL bPrimaryEnough = FALSE);

   private:
    VS_FIXEDFILEINFO m_FileInfo;

    std::wstring m_strCompanyName;
    std::wstring m_strFileDescription;
    std::wstring m_strFileVersion;
    std::wstring m_strInternalName;
    std::wstring m_strLegalCopyright;
    std::wstring m_strOriginalFileName;
    std::wstring m_strProductName;
    std::wstring m_strProductVersion;
    std::wstring m_strComments;
    std::wstring m_strLegalTrademarks;
    std::wstring m_strPrivateBuild;
    std::wstring m_strSpecialBuild;

    FILETIME m_ftCreationTime;
    FILETIME m_ftLastAccessTime;
    FILETIME m_ftLastWriteTime;
};
}  // namespace win
}  // namespace ashe
#endif  // !ASHE_WIN_FILE_INFO_H__