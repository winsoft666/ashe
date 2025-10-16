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
#ifndef ASHE_SINGLETON_PROCESS_HPP__
#define ASHE_SINGLETON_PROCESS_HPP__
#include "ashe/config.h"
#include "ashe/arch.h"
#include "ashe/macros.h"
#include <string>
#include <future>
#include "ashe/event.h"

namespace ashe {
// 进程单实例类，用于确保某个程序在系统中只能运行一个实例
// 
// 使用方法：
// SingletonProcess sp;
// sp.markAndCheckStartup("unique_name_for_your_program");
// if(!sp.isPrimary()) {
// }
//
class ASHE_API SingletonProcess {
   public:
    SingletonProcess();
    ~SingletonProcess();

    using SecondaryCallback = std::function<void(std::string secondaryCml)>;

    // 在进程启动时调用此函数以标记程序启动，并检查唯一实例
    // 程序应该在启动后尽快调用此函数
    // 
    // uniqueName参数用于标识唯一实例的名称，不能和其他程序重复
    //
    void markAndCheckStartup(const std::string& uniqueName);

    // 当第2个进程启动时，将调用 SecondaryCallback 并传入第2个进程的命令行参数
    // 命令行字符串采用 UTF-8 编码
    // 
    // SecondaryCallback 仅在 Windows 平台上实现
    //
    inline void registerSecondaryCallback(SecondaryCallback cb) { cb_ = cb; }

    const std::string& uniqueName() const;

    // 检查当前进程是否为第一个实例
    //
    bool isPrimary() const;

#ifdef ASHE_WIN
    void* mutex() const;
#else
    int pidFile() const;
#endif
   protected:
    void check();

    std::string uniqueName_;
    bool isPrimary_ = true;
#ifdef ASHE_WIN
    void* mutex_ = NULL;
    void* secondaryStartupEvent_ = NULL;
    void* fileMapping_ = NULL;
    void* viewOfFile_ = NULL;
    std::future<void> asyncQuery_;
#else
    int pidFile_ = -1;
#endif
    SecondaryCallback cb_;
    Event exit_;

    ASHE_DISALLOW_COPY_AND_MOVE(SingletonProcess);
};
}  // namespace ashe
#endif  // !ASHE_SINGLETON_PROCESS_HPP__
