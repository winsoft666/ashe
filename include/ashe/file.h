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

#ifndef ASHE_FILE_UTIL_HPP_
#define ASHE_FILE_UTIL_HPP_
#include "ashe/config.h"
#include "ashe/arch.h"
#include <stdio.h>
#include <mutex>
#include <string>
#include <vector>
#include "ashe/macros.h"
#include "ashe/path.h"
#ifdef ASHE_WIN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#endif  // ASHE_WIN

namespace ashe {
// 线程安全的文件读写类
//
class ASHE_API File {
   public:
    File(const Path& path);

    virtual ~File();

    Path path() const;

    bool isOpen();

    // [r]  读操作，文件必须已经存在
    //
    // [w]  写操作，创建一个空文件，如果同名文件已存在，则清空原有内容
    //
    // [a]  附加写操作，文件指针总是指向文件末尾（忽略fseek, fsetpos, rewind等操作），如果文件不存在则创建该文件
    //
    // [r+] 读/写操作，文件必须已经存在
    //
    // [w+] 读/写操作，创建一个空文件进行读写，如果同名文件已存在，则清空原有内容
    //
    // [a+] 附加读写操作，如果文件不存在则创建该文件。进行写操作时，始终从文件尾开始写；读操作则从文件当前指针位置开始读
    //
    // [b]  二进制模式，如rb, wb, ab, r+b, w+b, a+b 或 rb+, wb+, ab+
    //
    // 参考: https://www.cplusplus.com/reference/cstdio/fopen/
    //
    bool open(const std::wstring& openMode);
    bool open(const std::string& openMode);

    bool close();

    bool flush();

    // 检查文件是否存在
    // 无需调用 open(...) 打开文件
    //
    bool isExist() const;

    bool canRW() const;

    // 获取文件大小，单位字节
    // 需要先调用 open(...) 打开文件
    // 失败时返回 -1
    //
    int64_t fileSize();

    // 从当前文件指针位置偏移 offset 个字节
    //
    bool seekFromCurrent(int64_t offset);

    // 获取当前文件指针位置
    // 失败时返回 -1
    // 
    int64_t currentPointerPos();

    // 从文件开头偏移 offset 个字节
    //
    bool seekFromBeginning(int64_t offset);

    // 从文件结尾偏移 offset 个字节
    // offset 通常为负数
    //
    bool seekFromEnd(int64_t offset);

    // 从文件指定位置（from）读取 needRead 字节到 buffer 中
    // 调用者需要分配和释放 buffer
    // 
    // 如果 from 为 -1，则从当前文件指针位置读取
    // 该方法会改变文件指针位置
    // 
    // 返回实际读取的字节数，失败为0
    //
    size_t readFrom(void* buffer, size_t needRead, int64_t from = -1);

    // 从指定位置（from）写入 needRead 字节到文件中
    //
    // 如果 from 为 -1，则从当前文件指针位置开始写入
    // 该方法会改变文件指针位置
    // 
    // 返回实际写入的字节数，失败为0
    //
    size_t writeFrom(const void* buffer, size_t needWrite, int64_t from = -1);

    // 读取文件所有内容到 buffer 中
    // 
    // 该方法不会改变文件指针位置
    //
    bool readAll(std::vector<uint8_t>& buffer);

    // 读取文件所有内容
    // 读取失败返回空字符串
    //
    // 该方法不会改变文件指针位置
    //
    std::string readAll();

    // 读取文件所有内容
    //
    // 该方法不会改变文件指针位置
    //
    bool readAll(std::string& ret);

   protected:
    FILE* f_ = nullptr;
    Path path_;
    std::recursive_mutex mutex_;

    ASHE_DISALLOW_COPY_AND_MOVE(File);
};
}  // namespace ashe
#endif  // !ASHE_FILE_UTIL_HPP_
