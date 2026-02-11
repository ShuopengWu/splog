#include "splog.h"
#include <string>

static splog::Log& logger = splog::Log::instance(); 

namespace app {
class A{
public:
    void showLog(std::string s)
    {
        logger.add_log(s, std::source_location::current());
        logger.add_log({
                               {"name", "xiaoli", 0, splog::Alignment::Center, "[", "]", true},
                               {"id", "1", 0, splog::Alignment::Center, "[", "]", true}
                              },
                              std::source_location::current(), splog::LogLevel::Warning, ",");
    }
};
}

int main()
{
    {
        logger.set_sync_model(splog::LogSyncMode::Async);
        logger.set_output_target(splog::LogOutputTarget::All);
        logger.set_filename("1.log");
        logger.set_flush_threshold(100);
        logger.set_max_level(splog::LogLevel::Warning);
    }
    app::A a;
    std::string s;

    for (int i = 0; i < 100; i++)
        a.showLog(std::to_string(i));
}
