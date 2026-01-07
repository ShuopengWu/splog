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
    pimpl->configation.mode = mode;
    if (!pimpl->first_apply)
        pimpl->apply_configation(LogConfigationFlag::mode);
}

void splog::Log::set_output_target(LogOutputTarget target)
{
    pimpl->target = target;
    if (!pimpl->first_apply)
        pimpl->apply_configation(LogConfigationFlag::target);
}

void splog::Log::set_filename(const std::string &filename)
{
    pimpl->configation.filename = filename;
    if (!pimpl->first_apply)
        pimpl->apply_configation(LogConfigationFlag::filename);
}

void splog::Log::set_flush_threshold(int flush_threshold)
{
    pimpl->configation.flush_threshold = flush_threshold;
    if (!pimpl->first_apply)
        pimpl->apply_configation(LogConfigationFlag::flush_threshold);
}

void splog::Log::set_max_level(LogLevel l)
{
    pimpl->configation.max_level = l;
    if (!pimpl->first_apply)
        pimpl->apply_configation(LogConfigationFlag::max_level);
}

void splog::Log::add_log(const std::string &log, LogFuctionBuilder function, LogLevel l, const std::string &separator)
{
    if (pimpl->first_apply)
    {
        pimpl->apply_configation(LogConfigationFlag::all);
        pimpl->first_apply = false;
    }

    pimpl->client->add_log(log, l, separator, function.func);
    if (pimpl->target & LogOutputTarget::File)
    {
        if (!pimpl->server)
            std::cerr << "Please set the mode." << std::endl;
    }
}

void splog::Log::add_log(std::initializer_list<LogParameter> log, LogFuctionBuilder function, LogLevel l, const std::string &separator)
{
    if (pimpl->first_apply)
    {
        pimpl->apply_configation(LogConfigationFlag::all);
        pimpl->first_apply = false;
    }

    LogFormater f;
    for (const auto &l : log)
    {
        f[l.key] = l.value;
        f[l.key].min_width = l.width;
        f[l.key].alignment = l.align;
        f[l.key].is_show_key = l.is_show_key;
        f[l.key].prefix = l.prefix;
        f[l.key].suffix = l.suffix;
    }

    pimpl->client->add_log(f, l, separator, function.func);
}

splog::Log::Log()
{
    pimpl = new LogImpl();
    pimpl->client = std::make_unique<LogClient>();
}

splog::Log::~Log()
{
    delete pimpl;
}
