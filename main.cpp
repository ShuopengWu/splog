#include "splog.h"
#include <string>

namespace app {
class A{
public:
    void showLog(std::string s)
    {
        splog::logger.add_log(s, std::source_location::current());
        splog::logger.add_log({
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
        splog::logger.set_sync_model(splog::LogSyncMode::Async);
        splog::logger.set_output_target(splog::LogOutputTarget::All);
        splog::logger.set_filename("1.log");
        splog::logger.set_flush_threshold(1);
        splog::logger.set_max_level(splog::LogLevel::Warning);
    }
    app::A a;
    std::string s;

    for (int i = 0; i < 100000; i++)
        a.showLog(std::to_string(i));
}
