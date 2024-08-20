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

#ifndef ASHE_TIMEUTIL_HPP_
#define ASHE_TIMEUTIL_HPP_
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include <ctime>
#include <string>
#include <sstream>
#include <limits>
#include <chrono>

namespace ashe {
class ASHE_API Time {
   public:
    Time() {
        year = 0;
        month = 0;
        day = 0;
        hour = 0;
        minute = 0;
        second = 0;
        milliseconds = 0;
        microseconds = 0;
        nanoseconds = 0;
    }

    std::string toString(bool milli_precision = false,
                         bool micro_precision = false,
                         bool nano_precision = false) const;

   public:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int milliseconds;
    int microseconds;
    int nanoseconds;
};

#define kNumMillisecsPerSec INT64_C(1000)
#define kNumMicrosecsPerSec INT64_C(1000000);
#define kNumNanosecsPerSec INT64_C(1000000000);

#define kNumMicrosecsPerMillisec (kNumMicrosecsPerSec / kNumMillisecsPerSec)
#define kNumNanosecsPerMillisec (kNumNanosecsPerSec / kNumMillisecsPerSec)
#define kNumNanosecsPerMicrosec (kNumNanosecsPerSec / kNumMicrosecsPerSec)

// The microseconds that since 1970-01-01 00:00:00(UTC)
ASHE_API int64_t GetCurrentTimestampByMicroSec();

// The milliseconds that since 1970-01-01 00:00:00(UTC)
ASHE_API int64_t GetCurrentTimestampByMilliSec();

// The seconds that since 1970-01-01 00:00:00(UTC)
ASHE_API int64_t GetCurrentTimestampBySec();

// Windows: precision is milliseconds
ASHE_API Time GetLocalTime();

// Windows: precision is milliseconds
ASHE_API Time GetUTCTime();

#ifdef ASHE_WIN
Time FILETIMEToUTC(unsigned int dwLowDateTime,
                   unsigned int dwHighDateTime);
#endif
int64_t UTCToTimeStamp(Time t);

class ASHE_API TimeMeter {
   public:
    inline TimeMeter() {
        restart();
    }

    inline void restart() {
        lStartTime_ = std::chrono::high_resolution_clock::now();
    }

    // microseconds
    inline int64_t elapsed() const {
        auto now = std::chrono::high_resolution_clock::now();
        int64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(now - lStartTime_).count();
        return duration;
    }

   private:
    std::chrono::high_resolution_clock::time_point lStartTime_;
};
}  // namespace ashe
#endif  // !ASHE_TIMEUTIL_HPP_
