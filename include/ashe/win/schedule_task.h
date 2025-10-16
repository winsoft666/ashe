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

#ifndef ASHE_WIN_SCHEDULE_TASK_HPP__
#define ASHE_WIN_SCHEDULE_TASK_HPP__
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#include "ashe/macros.h"
#include <string>

namespace ashe {
namespace win {
// Windows 任务计划
//
class ASHE_API ScheduleTask {
   public:
    ScheduleTask();

    virtual ~ScheduleTask();

    // 根据名称删除计划任务
    //
    bool deleteTask(const wchar_t* pszTaskName);

    // 删除任务文件夹，文件夹下的任务也会被删除
    //
    bool deleteFolder(const wchar_t* pszFolderName);

    // 创建一个系统登录时（TASK_TRIGGER_LOGON）触发的计划任务
    //
    bool createLoginTriggerTask(const wchar_t* pszTaskName,
                                const wchar_t* pszProgramPath,
                                const wchar_t* pszParameters,
                                const wchar_t* pszDescription,
                                const wchar_t* pszAuthor);

    bool isExist(const wchar_t* pszTaskName);

    // 计划任务存在并且没有被禁用
    //
    bool isTaskValid(const wchar_t* pszTaskName);

    bool run(const wchar_t* pszTaskName, const wchar_t* pszParam);

    bool isEnabled(const wchar_t* pszTaskName);

    bool setEnabled(const wchar_t* pszTaskName, bool bEnable);

    // 获取计划任务操作的程序路径
    //
    bool getProgramPath(const wchar_t* pszTaskName, long lActionIndex, std::wstring& programPath);

    // 获取计划任务操作的程序参数
    //
    bool getParameters(const wchar_t* pszTaskName, long lActionIndex, std::wstring& parameters);

   protected:
    class Private;
    Private* p_ = nullptr;

    ASHE_DISALLOW_COPY_AND_MOVE(ScheduleTask);
};
}  // namespace win
}  // namespace ashe
#endif  // !ASHE_WIN_SCHEDULE_TASK_HPP__
