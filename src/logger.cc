#include "ashe/config.hpp"

#include "ashe/logger.hpp"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/async.h"
#include "spdlog/async_logger.h"
#include "ashe/macros.hpp"
#include "ashe/string_helper.hpp"

#ifndef SPDLOG_NO_EXCEPTIONS
#define SPDLOG_NO_EXCEPTIONS
#endif

#ifndef SPDLOG_WCHAR_FILENAMES
#define SPDLOG_WCHAR_FILENAMES
#endif

namespace ashe {
class Logger::Impl {
   public:
    std::shared_ptr<spdlog::async_logger> logger;
};

Logger::Logger(const std::string& loggerName, const std::wstring& baseFilePath) :
    impl_(new Impl()),
    baseFilePath_(baseFilePath) {
    assert(loggerName.length() <= 10);
    if (loggerName.length() > 10)
        loggerName_ = loggerName.substr(0, 10);
    else
        loggerName_ = loggerName;

    spdlog::init_thread_pool(8192, 2);
    std::vector<spdlog::sink_ptr> sinks;
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(baseFilePath_, 1048576, 7);
    file_sink->set_pattern("%Y-%m-%d %H:%M:%S.%e %z %-10n %-8l %v");
    sinks.push_back(file_sink);

#ifdef ASHE_WIN
    auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
    msvc_sink->set_pattern("%Y-%m-%d %H:%M:%S.%e %z %-10n %-8l %v");
    sinks.push_back(msvc_sink);
#endif

    impl_->logger = std::make_shared<spdlog::async_logger>(loggerName_,
                                                           sinks.begin(),
                                                           sinks.end(),
                                                           spdlog::thread_pool(),
                                                           spdlog::async_overflow_policy::block);
    impl_->logger->set_level(spdlog::level::trace);
    impl_->logger->flush_on(spdlog::level::info);
}

Logger::~Logger() {
    SAFE_DELETE(impl_);
}

std::string Logger::loggerName() const {
    return loggerName_;
}

std::wstring Logger::baseFilePath() const {
    return baseFilePath_;
}

void Logger::info(const char* format, ...) {
    std::string output;
    va_list args;
    va_start(args, format);
    const bool ret = StringHelper::StringPrintfV(format, args, output);
    va_end(args);

    if (ret)
        impl_->logger->info(output);
}

void Logger::warn(const char* format, ...) {
    std::string output;
    va_list args;
    va_start(args, format);
    const bool ret = StringHelper::StringPrintfV(format, args, output);
    va_end(args);

    if (ret)
        impl_->logger->warn(output);
}

void Logger::error(const char* format, ...) {
    std::string output;
    va_list args;
    va_start(args, format);
    const bool ret = StringHelper::StringPrintfV(format, args, output);
    va_end(args);

    if (ret)
        impl_->logger->error(output);
}

void Logger::critical(const char* format, ...) {
    std::string output;
    va_list args;
    va_start(args, format);
    const bool ret = StringHelper::StringPrintfV(format, args, output);
    va_end(args);

    if (ret)
        impl_->logger->critical(output);
}

}  // namespace ashe