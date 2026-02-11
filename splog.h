#ifndef SPLOG_H
#define SPLOG_H

#include <initializer_list>
#include <string>

#if __cplusplus >= 202002L
#include <source_location>
#define HAS_CXX20
#endif

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

enum class LogLevel : int
{
    Error = 0,
    Info,
    Warning,
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

enum class Alignment
{
    Left,
    Right,
    Center
};

struct LogParameter {
    std::string key;
    std::string value;
    int width = 0;
    Alignment align = Alignment::Left;
    std::string prefix = "";
    std::string suffix = "";
    bool is_show_key = true;
};

struct LogFuctionBuilder {
    const char* func;
    LogFuctionBuilder(const char* f = "unknown") : func(f) {}
#if __cplusplus >= 202002L
    LogFuctionBuilder(const std::source_location& loc) : func(loc.function_name()) {}
#endif
};

class SPLOG_API Log
{
public:
    static Log &instance();
    void set_sync_model(LogSyncMode mode);
    void set_output_target(LogOutputTarget target);
    void set_filename(const std::string &filename);
    void set_flush_threshold(int flush_threshold);
    void set_max_level(LogLevel l);
    void add_log(const std::string &log, LogFuctionBuilder function = {}, LogLevel l = LogLevel::Info, const std::string &separator = " ");
    void add_log(std::initializer_list<LogParameter> log, LogFuctionBuilder function = {}, LogLevel l = LogLevel::Info, const std::string &separator = " ");
private:
    Log();
    ~Log();
    class LogImpl;
    LogImpl *pimpl;
};

}

#endif // SPLOG_H
