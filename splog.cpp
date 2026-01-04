#include "splog.h"
#include "splog_p.h"
#include <iostream>

splog::Log &splog::Log::instance()
{
    static splog::Log log;
    return log;
}

void splog::Log::set_sync_model(LogSyncMode mode)
{
    if (mode == LogSyncMode::Sync)
        pimpl->server = std::make_unique<SyncLogServer>();
    else
        pimpl->server = std::make_unique<AsyncLogServer>();

    pimpl->client->set_handler([this](std::string data) {
        pimpl->server->sink(std::move(data));
    });
}

void splog::Log::set_output_target(LogOutputTarget target)
{
    pimpl->target = target;
    pimpl->client->set_target(target);
}

void splog::Log::set_filename(std::string_view filename)
{
    if (pimpl->target & LogOutputTarget::File)
    {
        if (pimpl->server)
            pimpl->server->set_file_name(filename);
        else
            std::cerr << "Please set the mode." << std::endl;
    }
    else
    {
        std::cerr << "Please enable file output before filename settings." << std::endl;
    }
}

void splog::Log::set_flush_threshold(int flush_threshold)
{
    if (pimpl->server)
        pimpl->server->set_flush_threshold(flush_threshold);
    else
        std::cerr << "Please set the mode." << std::endl;
}

void splog::Log::add_log(std::string_view log, LogLevel l, const std::source_location &location)
{
    pimpl->client->add_log(log, l, location);
    if (pimpl->target & LogOutputTarget::File)
    {
        if (!pimpl->server)
            std::cerr << "Please set the mode." << std::endl;
    }
}

// void splog::Log::add_log(LogFormater log, LogLevel l, const std::source_location &location)
// {
//     client->add_log(log, l, location);
//     if (target & LogOutputTarget::File)
//     {
//         if (!server)
//             std::cerr << "Please set the mode." << std::endl;

//     }
// }

splog::Log::Log()
{
    pimpl = new LogImpl();
    pimpl->client = std::make_unique<LogClient>();
}

splog::Log::~Log()
{
    delete pimpl;
}
