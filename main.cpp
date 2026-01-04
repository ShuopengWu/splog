#include "log.h"
#include <iostream>

namespace app {
class A{
public:
    void showLog(std::string s)
    {
        logger.add_log(s);
    }
};
}

int main()
{
    app::A a;
    std::string s;
    while (true)
    {
        std::cin >> s;
        a.showLog(s);
    }
}
