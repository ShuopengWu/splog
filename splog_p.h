#ifndef SPLOG_P_H
#define SPLOG_P_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <condition_variable>
#include "file.h"
#include "splog.h"

namespace splog
{

std::string to_string(LogLevel l);

struct LogElement
{
    std::string content = "";
    int min_width = 0;
    Alignment alignment = Alignment::Left;
    bool is_show_key = true;
    std::string prefix;
    std::string suffix;
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
    void set_separator(std::string_view separator);
private:
    std::string separator;
    std::vector<std::string> keys;
    std::unordered_map<std::string, LogElement> hash;
};

class LogClient
{
    using SinkHandler = std::function<void(std::string)>;
public:
    LogClient(LogOutputTarget target = LogOutputTarget::Console);
    void set_target(LogOutputTarget target);
    void set_max_level(LogLevel l);
    LogClient &add_log(std::string_view log, LogLevel l = LogLevel::Info, std::string_view separator = " ", std::string_view function_name = "");
    LogClient &add_log(LogFormater log, LogLevel l = LogLevel::Info, std::string_view separator = " ", std::string_view function_name = "");
    void set_handler(SinkHandler handler);
private:
    std::string extract_buffer();
    splog::LogFormater base_log(std::string_view function, LogLevel l);
    void show_to_console(std::string_view log);
    LogLevel max_level;
    std::string buffer;
    LogOutputTarget target;
    SinkHandler handler;
};

class LogServer
{
public:
    LogServer(std::string_view filename = "");
    virtual ~LogServer();
    void set_file_name(std::string_view filename);
    void set_max_threshold(int max_threshold);
    virtual void flush() = 0;
    virtual void sink(std::string &&data) = 0;
protected:
    int max_threshold;
    virtual void full_handler();
    file::File file;
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
    void set_flush_threshold(int flush_threshold);
private:
    int flush_threshold;
    std::atomic_bool is_stop;
    std::condition_variable cv;
    std::mutex mutex;
    std::thread worker;
};

struct LogConfiguration
{
    LogSyncMode mode;
    LogOutputTarget target;
    int flush_threshold;
    std::string filename;
    LogLevel max_level;
    LogConfiguration();
};

enum LogConfigurationFlag
{
    mode                = 0b000001,
    target              = 0b000010,
    flush_threshold     = 0b000100,
    filename            = 0b001000,
    max_level           = 0b010000,
    all                 = mode | target | flush_threshold | filename | max_level
};

class Log::LogImpl
{
public:
    void apply_configuration(LogConfigurationFlag flag);
    bool first_apply = true;
    LogConfiguration configuration;
    std::unique_ptr<LogClient> client;
    std::unique_ptr<LogServer> server;
};

}

#endif
