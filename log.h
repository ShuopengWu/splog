#ifndef LOG_H
#define LOG_H

#include <string>
#include <source_location>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <condition_variable>
#include "file.h"

namespace splog
{

enum class Alignment
{
    Left,
    Right,
    Center
};

struct LogElement
{
    std::string content = "";
    int min_width = 0;
    Alignment alignment = Alignment::Left;
    bool is_show_key = true;
    explicit LogElement(std::string_view key);
    explicit LogElement();
    std::string format();
    void operator =(std::string_view str);
    LogElement &operator =(const LogElement &element);
private:
    std::string key;
};

class LogFormater
{
public:
    std::string format();
    std::string to_string();
    LogElement &operator [](std::string_view key);
    LogFormater &operator +=(const LogFormater &formater);
private:
    std::vector<std::string> keys;
    std::unordered_map<std::string, LogElement> hash;
};

enum class LogLevel
{
    Info,
    Warning,
    Error,
    Debug
};

std::string to_string(LogLevel l);

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

class LogClient
{
    using SinkHandler = std::function<void(std::string)>;
public:
    LogClient(LogOutputTarget target = LogOutputTarget::Console);
    void set_target(LogOutputTarget target);
    LogClient &add_log(std::string_view log, LogLevel l = LogLevel::Info, const std::source_location &location = std::source_location::current());
    LogClient &add_log(LogFormater log, LogLevel l = LogLevel::Info, const std::source_location &location = std::source_location::current());
    void set_handler(SinkHandler handler);
private:
    std::string extract_buffer();
    splog::LogFormater base_log(std::string_view function, LogLevel l);
    void show_to_console(std::string_view log);
    std::string buffer;
    LogOutputTarget target;
    SinkHandler handler;
};

class LogServer
{
public:
    LogServer(std::string_view filename = "", int flush_threshold = 1024 * 1024);
    virtual ~LogServer();
    void set_file_name(std::string_view filename);
    void set_flush_threshold(int flush_threshold);
    virtual void flush() = 0;
    virtual void sink(std::string &&data) = 0;
protected:
    virtual void full_handler();
    file::File file;
    int flush_threshold;
    int max_threshold;
    std::string buffer;
};

class SyncLogServer : public LogServer
{
public:
    ~SyncLogServer();
    void flush();
    void sink(std::string &&data);
};

class AsyncLogServer : public LogServer
{
public:
    AsyncLogServer(std::string_view filename = "", int flush_threshold = 1024 * 1024);
    ~AsyncLogServer();
    void flush();
    void sink(std::string &&data);
private:
    std::atomic_bool is_stop;
    std::condition_variable cv;
    std::mutex mutex;
    std::thread worker;
};

class Log
{
public:
    static Log &instance();
    void set_sync_model(LogSyncMode mode);
    void set_output_target(LogOutputTarget target);
    void set_filename(std::string_view filename);
    void set_flush_threshold(int flush_threshold);
    void add_log(std::string_view log, LogLevel l = LogLevel::Info, const std::source_location &location = std::source_location::current());
    void add_log(LogFormater log, LogLevel l = LogLevel::Info, const std::source_location &location = std::source_location::current());
private:
    Log();
    LogOutputTarget target;
    std::unique_ptr<LogClient> client;
    std::unique_ptr<LogServer> server;
};

}

using namespace splog;
static Log &logger = splog::Log::instance();

#endif
