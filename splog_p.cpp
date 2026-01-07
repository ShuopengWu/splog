#include "splog_p.h"
#include <chrono>
#include <iostream>

splog::LogElement::LogElement(std::string_view key)
    : key(key)
{

}

splog::LogElement::LogElement()
{

}

std::string splog::LogElement::format()
{
    std::string core = is_show_key ? (std::string(key) + ":" + content) : content;
    std::string full_content = prefix + core + suffix;

    int pad = min_width - (int)full_content.size();

    if (pad <= 0) return full_content;

    std::string str(min_width, ' ');
    size_t start_pos = 0;

    switch (alignment)
    {
    case splog::Alignment::Left:   start_pos = 0; break;
    case splog::Alignment::Right:  start_pos = pad; break;
    case splog::Alignment::Center: start_pos = pad / 2; break;
    }

    std::copy(full_content.begin(), full_content.end(), str.begin() + start_pos);
    return str;
}

void splog::LogElement::operator =(std::string_view str)
{
    this->content = str;
}

splog::LogElement &splog::LogElement::operator =(const LogElement &element)
{
    this->content       = element.content;
    this->min_width     = element.min_width;
    this->alignment     = element.alignment;
    this->is_show_key   = element.is_show_key;
    this->key           = element.key;
    this->prefix        = element.prefix;
    this->suffix        = element.suffix;

    return *this;
}

std::string splog::LogFormater::format()
{
    std::string str;
    long long size = 0;

    for (int i = 0; i < keys.size(); i++)
        size += hash[keys[i]].min_width;

    str.reserve(size);

    for (int i = 0; i < keys.size(); i++)
    {
        if (i != 0)
            str += separator;
        str += hash[keys[i]].format();
    }

    str += '\n';

    return str;
}

std::string splog::LogFormater::to_string()
{
    return format();
}

splog::LogElement &splog::LogFormater::operator [](std::string_view key)
{
    std::string k(key);
    if (hash.find(k) == hash.end())
    {
        keys.push_back(k);
        hash[k] = LogElement(key);
    }
    return hash[k];
}

// If duplicate keys exist, only the original key will be retained.
splog::LogFormater &splog::LogFormater::operator +=(const LogFormater &formater)
{
    for (const auto &key : formater.keys)
    {
        if (hash.find(key) == hash.end())
        {
            auto it = formater.hash.find(key);
            if (it != formater.hash.end()) {
                this->keys.push_back(key);
                this->hash[key] = it->second;
            }
        }
    }

    return *this;
}

void splog::LogFormater::set_separator(std::string_view separator)
{
    this->separator = separator;
}

splog::LogClient::LogClient(LogOutputTarget target) :
    target(target)
{
}

void splog::LogClient::set_target(LogOutputTarget target)
{
    this->target = target;
}

void splog::LogClient::set_max_level(LogLevel l)
{
    max_level = l;
}

splog::LogClient &splog::LogClient::add_log(std::string_view log, LogLevel l, std::string_view separator, std::string_view function_name)
{
    if (l > max_level)
        return *this;

    splog::LogFormater base = base_log(function_name, l);
    base["message"] = log;
    base["message"].is_show_key = false;
    base.set_separator(separator);
    std::string all_log = base.format();
    if (target & LogOutputTarget::Console)
    {
        show_to_console(all_log);
    }
    if (target & LogOutputTarget::File)
    {
        buffer += all_log;
        if (handler)
        {
            handler(extract_buffer());
        }
    }
    else
    {
        buffer.clear();
    }
    return *this;
}

splog::LogClient &splog::LogClient::add_log(LogFormater log, LogLevel l, std::string_view separator, std::string_view function_name)
{
    if (l > max_level)
        return *this;

    splog::LogFormater base = base_log(function_name, l);
    base += log;
    base.set_separator(separator);
    std::string all_log = base.format();
    if (target & LogOutputTarget::Console)
    {
        show_to_console(all_log);
    }
    if (target & LogOutputTarget::File)
    {
        buffer += all_log;
        if (handler)
        {
            handler(extract_buffer());
        }
    }
    else
    {
        buffer.clear();
    }
    return *this;
}

std::string splog::LogClient::extract_buffer()
{
    std::string s;
    std::swap(s, buffer);
    return s;
}

void splog::LogClient::set_handler(SinkHandler handler)
{
    this->handler = handler;
}

splog::LogFormater splog::LogClient::base_log(std::string_view function, LogLevel l)
{
    splog::LogFormater log;

    std::chrono::time_point now = std::chrono::system_clock::now();
    auto const zone = std::chrono::current_zone();
    auto now_ms = std::chrono::floor<std::chrono::milliseconds>(zone->to_local(now));
    log["time"] = std::format("{:%F %T}", now_ms);
    log["time"].min_width = 23;

    log["function"] = function;
    log["function"].min_width = 30;
    log["level"] = to_string(l);
    log["level"].min_width = 13;

    return log;
}

void splog::LogClient::show_to_console(std::string_view log)
{
    std::cout << log;
}

std::string splog::to_string(LogLevel l)
{
    switch(l)
    {
    case splog::LogLevel::Info:     return "info";
    case splog::LogLevel::Warning:  return "warning";
    case splog::LogLevel::Error:    return "error";
    case splog::LogLevel::Debug:    return "debug";
    default:
        return "";
    }
}

splog::LogServer::LogServer(std::string_view filename) :
    file(filename)
{
    max_threshold = 1024 * 1024 * 4;
}

splog::LogServer::~LogServer()
{
}

void splog::LogServer::set_file_name(std::string_view filename)
{
    this->file.set_file_name(filename);
}

void splog::LogServer::set_max_threshold(int max_threshold)
{
    this->max_threshold = max_threshold;
}

void splog::LogServer::full_handler()
{
    std::cerr << "[splog]Files and multiple write failures will trigger the cleanup of old logs to prevent excessive memory usage." << std::endl;
    std::size_t target_size = max_threshold * 0.8;
    if (buffer.size() < target_size)
        return;

    while (buffer.size() > target_size)
    {
        std::size_t i = buffer.find('\n');

        if (i == std::string::npos)
        {
            buffer.clear();
            return;
        }

        buffer.erase(0, i + 1);
    }
}

splog::SyncLogServer::~SyncLogServer()
{
    SyncLogServer::flush();
}

void splog::SyncLogServer::flush()
{
    if (buffer.empty())
        return;

    bool ok = false;

    static constexpr int RETRY_COUNT = 3;
    for (int i = 0; i < RETRY_COUNT; i++)
    {
        ok = file.write(buffer, file::Write_Mode::Append);
        if (ok)
        {
            buffer.clear();
            return;
        }
    }

    if (buffer.size() > max_threshold)
        full_handler();
}

void splog::SyncLogServer::sink(std::string &&data)
{
    if (data.empty())
        return;
    buffer.append(data);
    flush();
}

splog::AsyncLogServer::AsyncLogServer(std::string_view filename, int flush_threshold)
    : LogServer(filename),
    is_stop(false)
{
    worker = std::thread([this]{
        std::string write_buffer;

        while (true)
        {
            write_buffer.clear();
            {
                std::unique_lock<std::mutex> lock(mutex);

                cv.wait_for(lock, std::chrono::seconds(3), [this]{
                    return is_stop || buffer.size() >= this->flush_threshold;
                });

                if (buffer.empty())
                {
                    if (is_stop)
                        return;
                    continue;
                }

                write_buffer.swap(buffer);
                buffer.reserve(this->flush_threshold);
            }

            if (!write_buffer.empty())
                file.write(write_buffer, file::Write_Mode::Append);

            if (is_stop && buffer.empty())
                return;
        }
    });
}

splog::AsyncLogServer::~AsyncLogServer()
{
    is_stop = true;
    cv.notify_all();

    if (worker.joinable())
        worker.join();
}

void splog::AsyncLogServer::flush()
{
    cv.notify_one();
}

void splog::AsyncLogServer::sink(std::string &&data)
{
    if (data.empty())
        return;

    {
        std::unique_lock<std::mutex> lock(mutex);
        buffer.append(data);

        if (buffer.size() > max_threshold)
            cv.notify_one();
    }
}

void splog::AsyncLogServer::set_flush_threshold(int flush_threshold)
{
    this->flush_threshold = flush_threshold;
    static constexpr int MAGNIFICATION = 4;
    this->max_threshold = MAGNIFICATION * flush_threshold;
}

splog::LogConfiguration::LogConfiguration()
{
    mode = LogSyncMode::Sync;
    target = LogOutputTarget::Console;
    flush_threshold = 1;
    filename = "";
    max_level = LogLevel::Warning;
}

void splog::Log::LogImpl::apply_configuration(LogConfigurationFlag flag)
{
    if (flag & LogConfigurationFlag::mode)
    {
        if (!server)
        {
            if (configuration.mode == LogSyncMode::Sync)
                server = std::make_unique<SyncLogServer>();
            else
                server = std::make_unique<AsyncLogServer>();

            client->set_handler([this](std::string data){
                server->sink(std::move(data));
            });
        }

        // The mode switching function will be implemented later.
    }

    if (flag & LogConfigurationFlag::target)
    {
        client->set_target(configuration.target);
    }

    if (flag & LogConfigurationFlag::filename)
    {
        if (server)
            server->set_file_name(configuration.filename);
    }

    if (flag & LogConfigurationFlag::flush_threshold)
    {
        if (server)
        {
            auto *async_server = dynamic_cast<AsyncLogServer *>(server.get());
            if (async_server)
                async_server->set_flush_threshold(configuration.flush_threshold);
        }
    }

    if (flag & LogConfigurationFlag::max_level)
    {
        client->set_max_level(configuration.max_level);
    }
}
