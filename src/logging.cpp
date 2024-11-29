#include "ashe/logging.h"
#include "ashe/debug.h"
#include <fstream>
#include <iomanip>
#include <mutex>
#include <ostream>
#include <thread>
#include "ashe/time_util.h"
#if defined(ASHE_WIN)
#include "ashe/win/windows_lite.h"
#include <Psapi.h>
#include "ashe/win/dbg_trace.h"
#include "ashe/string_encode.h"
#endif  // defined(ASHE_WIN)
#include "ashe/byteorder.h"
#if defined(ASHE_LINUX)
#include <linux/limits.h>
#include <unistd.h>
#endif  // defined(ASHE_LINUX)

#if defined(ASHE_MACOS)
#include <mach-o/dyld.h>
#include <sys/syslimits.h>
#endif  // defined(ASHE_MACOS)

namespace ashe {

namespace {

const LoggingSeverity kDefaultLogLevel = LOG_LS_FATAL;
const size_t kDefaultMaxLogFileSize = 2 * 1024 * 1024;  // 2 Mb.
const size_t kDefaultMaxLogFileAge = 14;                // 14 days.

LoggingSeverity g_min_log_level = LOG_LS_ERROR;
LoggingDestination g_logging_destination = LOG_DEFAULT;

size_t g_max_log_file_size = kDefaultMaxLogFileSize;
size_t g_max_log_file_age = kDefaultMaxLogFileAge;
int g_log_file_number = -1;

filesystem::path g_log_dir_path;
filesystem::path g_log_file_path;
std::ofstream g_log_file;
std::mutex g_log_file_lock;

const char* severityName(LoggingSeverity severity) {
    static const char* const kLogSeverityNames[] = {"I", "E", "F"};

    if (severity >= 0 && severity < 3)
        return kLogSeverityNames[severity];

    return "UNKNOWN";
}

void removeOldFiles(const filesystem::path& path, const filesystem::file_time_type& current_time, size_t max_file_age) {
    filesystem::file_time_type time = current_time - std::chrono::hours(24U * max_file_age);

    std::error_code ignored_code;
    for (const auto& item : filesystem::directory_iterator(path, ignored_code)) {
        if (item.is_directory())
            continue;

        if (item.last_write_time() < time)
            filesystem::remove(item.path(), ignored_code);
    }
}

filesystem::path defaultLogFileDir(const std::string& prefix) {
    std::error_code error_code;

    filesystem::path path = filesystem::temp_directory_path(error_code);
    if (error_code)
        return filesystem::path();

    path.append(prefix);
    return path;
}

bool initLoggingUnlocked(const std::string& prefix) {
    g_log_file.close();

    if (!(g_logging_destination & LOG_TO_FILE))
        return true;

    // The next log file must have a number higher than the current one.
    ++g_log_file_number;

    filesystem::path file_dir = g_log_dir_path;
    if (file_dir.empty())
        file_dir = defaultLogFileDir(prefix);

    if (file_dir.empty())
        return false;

    std::error_code error_code;
    if (!filesystem::exists(file_dir, error_code)) {
        if (error_code)
            return false;

        if (!filesystem::create_directories(file_dir, error_code))
            return false;
    }

    Time time = GetLocalTime();

    std::ostringstream file_name_stream;
    file_name_stream
        << std::setfill('0')
        << std::setw(4) << time.year
        << std::setw(2) << time.month
        << std::setw(2) << time.day
        << '-'
        << std::setw(2) << time.hour
        << std::setw(2) << time.minute
        << std::setw(2) << time.second
        << '.'
        << std::setw(3) << time.milliseconds
        << '.'
        << g_log_file_number
        << ".log";

    filesystem::path file_path(file_dir);
    file_path.append(file_name_stream.str());

    g_log_file.open(file_path);
    if (!g_log_file.is_open())
        return false;

    if (g_max_log_file_age != 0) {
        filesystem::file_time_type file_time =
            filesystem::last_write_time(file_path, error_code);
        if (!error_code)
            removeOldFiles(file_dir, file_time, g_max_log_file_age);
    }

    g_log_file_path = std::move(file_path);
    return true;
}

}  // namespace

// This is never instantiated, it's just used for EAT_STREAM_PARAMETERS to have
// an object of the correct type on the LHS of the unused part of the ternary
// operator.
std::ostream* g_swallow_stream;

LoggingSettings::LoggingSettings() :
    minLogLevel(kDefaultLogLevel),
    maxLogFileSize(kDefaultMaxLogFileSize),
    maxLogFileAge(kDefaultMaxLogFileAge) {
    int log_to_file = 0;
    if (LOG_DEFAULT & LOG_TO_FILE)
        log_to_file = 1;

    int log_to_stdout = 0;
    if (LOG_DEFAULT & LOG_TO_STDOUT)
        log_to_stdout = 1;

    if (log_to_file != 0 && log_to_stdout != 0)
        destination = LOG_TO_ALL;
    else if (log_to_file != 0 && log_to_stdout == 0)
        destination = LOG_TO_FILE;
    else if (log_to_file == 0 && log_to_stdout != 0)
        destination = LOG_TO_STDOUT;
    else
        destination = LOG_NONE;
}

filesystem::path execFilePath() {
    filesystem::path exec_file_path;

#if defined(ASHE_WIN)
    wchar_t buffer[MAX_PATH] = {0};
    GetModuleFileNameExW(GetCurrentProcess(), nullptr, buffer, static_cast<DWORD>(std::size(buffer)));
    exec_file_path = buffer;
#elif defined(ASHE_LINUX)
    char buffer[PATH_MAX] = {0};
    if (readlink("/proc/self/exe", buffer, PATH_MAX) == -1)
        return filesystem::path();
    exec_file_path = buffer;
#elif defined(ASHE_MACOS)
    char buffer[PATH_MAX] = {0};
    uint32_t buffer_size = PATH_MAX;
    _NSGetExecutablePath(buffer, &buffer_size);
    exec_file_path = buffer;
#else
#error Not implemented
#endif

    return exec_file_path;
}

std::string logFilePrefix() {
    filesystem::path exec_file_path = execFilePath();
    filesystem::path exec_file_name = exec_file_path.filename();
    exec_file_name.replace_extension();
    return exec_file_name.string();
}

bool initLogging(const LoggingSettings& settings) {
    {
        std::lock_guard<std::mutex> lock(g_log_file_lock);

        g_logging_destination = settings.destination;
        g_min_log_level = settings.minLogLevel;
        g_log_dir_path = settings.logDir;
        g_max_log_file_size = settings.maxLogFileSize;
        g_max_log_file_age = settings.maxLogFileAge;

        if (!initLoggingUnlocked(logFilePrefix()))
            return false;
    }

    LOG(LS_INFO) << "Executable file: " << execFilePath();
    if (g_logging_destination & LOG_TO_FILE) {
        // If log output is enabled, then we output information about the file.
        LOG(LS_INFO) << "Logging file: " << g_log_file_path;
    }
    LOG(LS_INFO) << "Logging level: " << g_min_log_level;
#ifdef ASHE_WIN
    LOG(LS_INFO) << "Debugger present: " << (isDebuggerPresent() ? "Yes" : "No");
#endif

#if defined(NDEBUG)
    LOG(LS_INFO) << "Debug build: No";
#else
    LOG(LS_INFO) << "Debug build: Yes";
#endif  // defined(NDEBUG)

    LOG(LS_INFO) << "Little endian: " << !ByteOrder::IsHostBigEndian();
    LOG(LS_INFO) << "Logging started";
    return true;
}

void shutdownLogging() {
    LOG(LS_INFO) << "Logging finished";

    std::lock_guard<std::mutex> lock(g_log_file_lock);
    g_log_file.close();
}

bool shouldCreateLogMessage(LoggingSeverity severity) {
    if (severity < g_min_log_level)
        return false;

    // Return true here unless we know ~LogMessage won't do anything. Note that
    // ~LogMessage writes to stderr if severity_ >= kAlwaysPrintErrorLevel, even
    // when g_logging_destination is LOG_NONE.
    return g_logging_destination != LOG_NONE || severity >= LOG_LS_ERROR;
}

LogMessage::LogMessage(const std::string& file,
                       int line,
                       const std::string& function,
                       LoggingSeverity severity) :
    severity_(severity) {
    init(file, line, function);
}

LogMessage::LogMessage(const std::string& file,
                       int line,
                       const std::string& function,
                       const char* condition) :
    severity_(LOG_LS_FATAL) {
    init(file, line, function);
    stream_ << "Check failed: " << condition << ". ";
}

LogMessage::~LogMessage() {
    stream_ << std::endl;

    std::string message(stream_.str());

    if ((g_logging_destination & LOG_TO_STDOUT) != 0) {
#ifdef ASHE_WIN
        win::TraceA(message.data());
#endif
        fwrite(message.data(), message.size(), 1, stderr);
        fflush(stderr);
    }
    else if (severity_ >= LOG_LS_ERROR) {
        // When we're only outputting to a log file, above a certain log level, we
        // should still output to stderr so that we can better detect and diagnose
        // problems with unit tests, especially on the buildbots.
        fwrite(message.data(), message.size(), 1, stderr);
        fflush(stderr);
    }

    // Write to log file.
    if ((g_logging_destination & LOG_TO_FILE) != 0) {
        std::lock_guard<std::mutex> lock(g_log_file_lock);

        if (static_cast<size_t>(g_log_file.tellp()) >= g_max_log_file_size) {
            // The maximum size of the log file has been exceeded. Close the current log file and
            // create a new one.
            initLoggingUnlocked(logFilePrefix());
        }

        g_log_file.write(message.c_str(), message.size());
        g_log_file.flush();
    }

    if (severity_ == LOG_LS_FATAL) {
        // Crash the process.
        debugBreak();
    }
}

// Writes the common header info to the stream.
void LogMessage::init(const std::string& file, int line, const std::string& function) {
    std::string fileFormat;
    size_t last_slash_pos = file.find_last_of("\\/");
    if (last_slash_pos != std::string::npos)
        fileFormat = file.substr(last_slash_pos + 1);
    else
        fileFormat = file;

    Time time = GetLocalTime();

    stream_ << std::setfill('0')
            << std::setw(2) << time.hour << ':'
            << std::setw(2) << time.minute << ':'
            << std::setw(2) << time.second << '.'
            << std::setw(3) << time.milliseconds << ' '
            << std::this_thread::get_id() << ' '
            << severityName(severity_) << " ["
            << fileFormat.data() << ":" << line << " " << function.data() << "] ";

    message_start_ = stream_.str().length();
}

ErrorLogMessage::ErrorLogMessage(const std::string& file,
                                 int line,
                                 const std::string& function,
                                 LoggingSeverity severity,
                                 SystemError error) :
    error_(error),
    log_message_(file, line, function, severity) {
    // Nothing
}

ErrorLogMessage::~ErrorLogMessage() {
    stream() << ": (" << error_.code() << ") " << error_.toString();
}
}  // namespace ashe

namespace std {
#if defined(ASHE_WIN)
std::ostream& operator<<(std::ostream& out, const std::wstring& wstr) {
    return out << ::ashe::w2u(wstr);
}

std::ostream& operator<<(std::ostream& out, const wchar_t* wstr) {
    return out << (wstr ? ::ashe::w2u(wstr) : "nullptr");
}
#endif  // defined(ASHE_WIN)
}  // namespace std
