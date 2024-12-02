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

#ifndef ASHE_LOGGING_H_
#define ASHE_LOGGING_H_
#pragma once

#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include "ashe/config.h"
#include "ashe/arch.h"
#include "ashe/scoped_clear_last_error.h"
#include "ashe/system_error.h"
#include "ashe/filesystem.h"

// Instructions
// ------------
//
// Make a bunch of macros for logging. The way to log things is to stream things to
// LOG(<a particular severity level>). E.g.,
//
//   LOG(LS_INFO) << "Found " << num_cookies << " cookies";
//
// You can also do conditional logging:
//
//   LOG_IF(LS_INFO, num_cookies > 10) << "Got lots of cookies";
//
// The CHECK(condition) macro is active in both debug and release builds and effectively performs a
// LOG(LS_FATAL) which terminates the process and generates a crashdump unless a debugger is attached.
//
// There are also "debug mode" logging macros like the ones above:
//
//   DLOG(LS_INFO) << "Found cookies";
//
//   DLOG_IF(LS_INFO, num_cookies > 10) << "Got lots of cookies";
//
// All "debug mode" logging is compiled away to nothing for non-debug mode compiles. LOG_IF and
// development flags also work well together because the code can be compiled away sometimes.
//
// We also have
//
//   LOG_ASSERT(assertion);
//   DLOG_ASSERT(assertion);
//
// which is syntactic sugar for {,D}LOG_IF(LS_FATAL, assert fails) << assertion;
//
// We also override the standard 'assert' to use 'DLOG_ASSERT'.
//
// Lastly, there is:
//
//   PLOG(LS_ERROR) << "Couldn't do foo";
//   DPLOG(LS_ERROR) << "Couldn't do foo";
//   PLOG_IF(LS_ERROR, cond) << "Couldn't do foo";
//   DPLOG_IF(LS_ERROR, cond) << "Couldn't do foo";
//   PCHECK(condition) << "Couldn't do foo";
//   DPCHECK(condition) << "Couldn't do foo";
//
// which append the last system error to the message in string form (taken from GetLastError() on
// Windows and errno on POSIX).
//
// The supported severity levels for macros that allow you to specify one are (in increasing order
// of severity) LS_INFO, LS_ERROR, and LS_FATAL.
//
// Very important: logging a message at the LS_FATAL severity level causes the program to terminate
// (after the message is logged).
//
// There is the special severity of DFATAL, which logs LS_FATAL in debug mode, LS_ERROR in normal mode.

namespace ashe {

enum LoggingDestination {
    LOG_NONE = 0,
    LOG_TO_FILE = 1 << 0,
    LOG_TO_STDOUT = 1 << 1,
    LOG_TO_ALL = LOG_TO_FILE | LOG_TO_STDOUT,

#if defined(ASHE_WIN)
// On Windows, use a file next to the exe.
#if defined(NDEBUG)
    LOG_DEFAULT = LOG_TO_FILE
#else   // defined(NDEBUG)
    LOG_DEFAULT = LOG_TO_ALL
#endif  // defined(NDEBUG)
#elif defined(ASHE_POSIX)
    LOG_DEFAULT = LOG_TO_STDOUT
#endif  // defined(ASHE_WIN)
};

enum LoggingSeverity {
    LOG_LS_INFO = 0,
    LOG_LS_ERROR = 1,
    LOG_LS_FATAL = 2
};

struct ASHE_API LoggingSettings {
    // The defaults values are:
    //
    //  destination: LOG_DEFAULT
    //  minLogLevel: LOG_LS_INFO
    //  maxLogFileSize: 2 Mb
    //  maxLogFileAge: 14 days
    LoggingSettings();

    LoggingDestination destination;
    LoggingSeverity minLogLevel;

    filesystem::path logDir;

    size_t maxLogFileSize;
    size_t maxLogFileAge;
};

class ASHE_API LogMessage {
   public:
    // Used for LOG(severity).
    LogMessage(const std::string& file, int line, const std::string& function, LoggingSeverity severity);

    // Used for CHECK(). Implied severity = LOG_FATAL.
    LogMessage(const std::string& file, int line, const std::string& function, const char* condition);

    ~LogMessage();

    std::ostream& stream() { return stream_; }

    LoggingSeverity severity() { return severity_; }
    std::string str() { return stream_.str(); }

   private:
    void init(const std::string& file, int line, const std::string& function);

    LoggingSeverity severity_;
    std::ostringstream stream_;

    // Offset of the start of the message (past prefix // info).
    size_t message_start_;

    ScopedClearLastError last_error_;

    ASHE_DISALLOW_COPY_AND_ASSIGN(LogMessage);
};

// This class is used to explicitly ignore values in the conditional logging macros. This avoids
// compiler warnings like "value computed is not used" and "statement has no effect".
class ASHE_API LogMessageVoidify {
   public:
    LogMessageVoidify() = default;
    // This has to be an operator with a precedence lower than << but higher than ?:
    void operator&(std::ostream&) {}
};

// Appends a formatted system message of the GetLastError() type.
class ASHE_API ErrorLogMessage {
   public:
    ErrorLogMessage(const std::string& file, int line, const std::string& function, LoggingSeverity severity, SystemError error);
    ~ErrorLogMessage();
    std::ostream& stream() { return log_message_.stream(); }

   private:
    SystemError error_;
    LogMessage log_message_;

    ASHE_DISALLOW_COPY_AND_ASSIGN(ErrorLogMessage);
};

// Sets the log file name and other global logging state. Calling this function is recommended,
// and is normally done at the beginning of application init.
// See the definition of the enums above for descriptions and default values.
ASHE_API bool initLogging(const LoggingSettings& settings = LoggingSettings());

// Closes the log file explicitly if open.
// NOTE: Since the log file is opened as necessary by the action of logging statements, there's no
//       guarantee that it will stay closed after this call.
ASHE_API void shutdownLogging();

// Used by LOG_IS_ON to lazy-evaluate stream arguments.
ASHE_API bool shouldCreateLogMessage(LoggingSeverity severity);

#if defined(NDEBUG) && !defined(ASHE_DCHECK_ALWAYS_ON)
#define ASHE_DCHECK_IS_ON() false
#else
#define ASHE_DCHECK_IS_ON() true
#endif

// A few definitions of macros that don't generate much code. These are used by LOG() and LOG_IF,
// etc. Since these are used all over our code, it's better to have compact code for these operations.
#define COMPACT_LOG_EX_LS_INFO(ClassName, ...) \
    ::ashe::ClassName(__FILE__, __LINE__, __FUNCTION__, ::ashe::LOG_LS_INFO, ##__VA_ARGS__)
#define COMPACT_LOG_EX_LS_ERROR(ClassName, ...) \
    ::ashe::ClassName(__FILE__, __LINE__, __FUNCTION__, ::ashe::LOG_LS_ERROR, ##__VA_ARGS__)
#define COMPACT_LOG_EX_LS_FATAL(ClassName, ...) \
    ::ashe::ClassName(__FILE__, __LINE__, __FUNCTION__, ::ashe::LOG_LS_FATAL, ##__VA_ARGS__)

#define COMPACT_LOG_LS_INFO COMPACT_LOG_EX_LS_INFO(LogMessage)
#define COMPACT_LOG_LS_ERROR COMPACT_LOG_EX_LS_ERROR(LogMessage)
#define COMPACT_LOG_LS_FATAL COMPACT_LOG_EX_LS_FATAL(LogMessage)

// As special cases, we can assume that LOG_IS_ON(LS_FATAL) always holds. Also, LOG_IS_ON(LS_DFATAL)
// always holds in debug mode. In particular, CHECK()s will always fire if they fail.
#define LOG_IS_ON(severity) \
    (::ashe::shouldCreateLogMessage(::ashe::LOG_##severity))

// Helper macro which avoids evaluating the arguments to a stream if the condition doesn't hold.
// Condition is evaluated once and only once.
#define LAZY_STREAM(stream, condition) \
    !(condition) ? (void)0 : ::ashe::LogMessageVoidify() & (stream)

// We use the preprocessor's merging operator, "##", so that, e.g., LOG(LS_INFO) becomes the token
// COMPACT_LOG_LS_INFO. There's some funny subtle difference between ostream member streaming
// functions (e.g., ostream::operator<<(int) and ostream non-member streaming functions
// (e.g., ::operator<<(ostream&, string&): it turns out that it's impossible to stream something
// like a string directly to an unnamed ostream. We employ a neat hack by calling the stream() member
// function of LogMessage which seems to avoid the problem.
#define LOG_STREAM(severity) \
    COMPACT_LOG_##severity.stream()

#define LOG(severity) \
    LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity))

#define LOG_IF(severity, condition) \
    LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity) && (condition))

#define LOG_ASSERT(condition) \
    LOG_IF(FATAL, !(condition)) << "Assert failed: " #condition ". "

#define PLOG_STREAM(severity) \
    COMPACT_LOG_EX_##severity(ErrorLogMessage, ::ashe::SystemError::last()).stream()

#define PLOG(severity) \
    LAZY_STREAM(PLOG_STREAM(severity), LOG_IS_ON(severity))

#define PLOG_IF(severity, condition) \
    LAZY_STREAM(PLOG_STREAM(severity), LOG_IS_ON(severity) && (condition))

extern std::ostream* g_swallow_stream;

// Note that g_swallow_stream is used instead of an arbitrary LOG() stream to avoid the creation of
// an object with a non-trivial destructor (LogMessage).
// On MSVC x86 (checked on 2015 Update 3), this causes a few additional pointless instructions to
// be emitted even at full optimization level, even though the : arm of the ternary operator is
// clearly never executed. Using a simpler object to be &'d with Voidify() avoids these extra
// instructions.
// Using a simpler POD object with a templated operator<< also works to avoid these instructions.
// However, this causes warnings on statically defined implementations of operator<<(std::ostream, ...)
// in some .cc files, because they become defined-but-unreferenced functions. A reinterpret_cast of
// 0 to an ostream* also is not suitable, because some compilers warn of undefined behavior.
#define EAT_STREAM_PARAMETERS \
    true ? (void)0 : ::ashe::LogMessageVoidify() & (*::ashe::g_swallow_stream)

// CHECK dies with a fatal error if condition is not true.  It is *not* controlled by NDEBUG, so
// the check will be executed regardless of compilation mode.
//
// We make sure CHECK et al. always evaluates their arguments, as doing
// CHECK(FunctionWithSideEffect()) is a common idiom.

// Do as much work as possible out of line to reduce inline code size.
#define CHECK(condition) \
    LAZY_STREAM(::ashe::LogMessage(__FILE__, __LINE__, __FUNCTION__, #condition).stream(), !(condition))

#define PCHECK(condition) \
    LAZY_STREAM(PLOG_STREAM(LS_FATAL), !(condition)) << "Check failed: " #condition ". "

#if ASHE_DCHECK_IS_ON()

#define DCHECK(condition) \
    LAZY_STREAM(LOG_STREAM(LS_FATAL), !(condition)) << "Check failed: " #condition ". "
#define DPCHECK(condition) \
    LAZY_STREAM(PLOG_STREAM(LS_FATAL), !(condition)) << "Check failed: " #condition ". "

#else  // ASHE_DCHECK_IS_ON()

#define DCHECK(condition) EAT_STREAM_PARAMETERS << !(condition)
#define DPCHECK(condition) EAT_STREAM_PARAMETERS << !(condition)

#endif  // ASHE_DCHECK_IS_ON()

#define NOTREACHED() DCHECK(false)

// Definitions for DLOG et al.
//
#if ASHE_DCHECK_IS_ON()

#define DLOG_IS_ON(severity) LOG_IS_ON(severity)
#define DLOG_IF(severity, condition) LOG_IF(severity, condition)
#define DLOG_ASSERT(condition) LOG_ASSERT(condition)
#define DPLOG_IF(severity, condition) PLOG_IF(severity, condition)

#else  // ASHE_DCHECK_IS_ON()

// If !DCHECK_IS_ON(), we want to avoid emitting any references to |condition| (which may reference
// a variable defined only if DCHECK_IS_ON()).
// Contrast this with DCHECK et al., which has different behavior.

#define DLOG_IS_ON(severity) false
#define DLOG_IF(severity, condition) EAT_STREAM_PARAMETERS
#define DLOG_ASSERT(condition) EAT_STREAM_PARAMETERS
#define DPLOG_IF(severity, condition) EAT_STREAM_PARAMETERS

#endif  // DCHECK_IS_ON()

#define DLOG(severity) LAZY_STREAM(LOG_STREAM(severity), DLOG_IS_ON(severity))
#define DPLOG(severity) LAZY_STREAM(PLOG_STREAM(severity), DLOG_IS_ON(severity))

}  // namespace ashe

// Note that "The behavior of a C++ program is undefined if it adds declarations or definitions to
// namespace std or to a namespace within namespace std unless otherwise specified.
//
// We've checked that this particular definition has the intended behavior on our implementations,
// but it's prone to breaking in the future, and please don't imitate this in your own definitions
// without checking with some standard library experts.
namespace std {
#if defined(ASHE_WIN)
ASHE_API std::ostream& operator<<(std::ostream& out, const wchar_t* wstr);
ASHE_API std::ostream& operator<<(std::ostream& out, const std::wstring& wstr);
#endif  // defined(ASHE_WIN)
}  // namespace std

#endif  // !ASHE_LOGGING_H_
