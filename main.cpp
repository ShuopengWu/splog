#include "splog.h"
#include <iostream>
#include <string>

namespace app {
class A{
public:
    void showLog(std::string s)
    {
        splog::logger.add_log(s);
    }
};
}

int main()
{
    {
        splog::logger.set_sync_model(splog::LogSyncMode::Sync);
        splog::logger.set_output_target(splog::LogOutputTarget::All);
        splog::logger.set_filename("1.log");
        splog::logger.set_flush_threshold(10000000);
    }
    app::A a;
    std::string s;

    for (int i = 0; i < 1000; i++)
        a.showLog(std::to_string(i));
}
