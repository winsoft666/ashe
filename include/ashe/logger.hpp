/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022 winsoft666 <winsoft666@outlook.com>.
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

#ifndef ASHE_LOGGER_HPP__
#define ASHE_LOGGER_HPP__
#include "ashe/config.hpp"
#include <string>

namespace ashe {
class ASHE_API Logger {
   public:
    Logger(const std::string& loggerName, const std::wstring& baseFilePath);
    virtual ~Logger();

    std::string loggerName() const;
    std::wstring baseFilePath() const;

    void info(const char* format, ...);
    void warn(const char* format, ...);
    void error(const char* format, ...);
    void critical(const char* format, ...);

   protected:
    std::string loggerName_;
    std::wstring baseFilePath_;

   private:
    class Impl;
    Impl* impl_ = nullptr;
};
}  // namespace ashe
#endif  //!ASHE_LOGGER_HPP__
