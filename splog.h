#ifndef SPLOG_H
#define SPLOG_H

#include <string_view>
#include <source_location>

#ifdef _WIN32
    #ifdef SPLOG_EXPORTS
        #define SPLOG_API __declspec(dllexport)
    #else
        #define SPLOG_API __declspec(dllimport)
    #endif
#else
    #define SPLOG_API
#endif

namespace splog {

enum class LogLevel
{
    Info,
    Warning,
    Error,
    Debug
};

enum LogOutputTarget
{
    Console = 0b01,
    File    = 0b10,
    All     = Console | File
};

enum class LogSyncMode {
    Sync,
    Async
};

class SPLOG_API Log
{
public:
    static Log &instance();
    void set_sync_model(LogSyncMode mode);
    void set_output_target(LogOutputTarget target);
    void set_filename(std::string_view filename);
    void set_flush_threshold(int flush_threshold);
    void add_log(std::string_view log, LogLevel l = LogLevel::Info, const std::source_location &location = std::source_location::current());
private:
    Log();
    ~Log();
    class LogImpl;
    LogImpl *pimpl;
};

static Log &logger = Log::instance();

}

#endif // SPLOG_H
