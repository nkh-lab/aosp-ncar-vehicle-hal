#define LOG_TAG "nlab-vhal"
#include <log/log_main.h>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

int main(int argc, char const* argv[])
{
    static_cast<void>(argc);
    static_cast<void>(argv);

    ALOGI("Hello World!\n");

    int32_t c = 0;

    for (;;)
    {
        ALOGI("Heart beat: %d", c++);
        std::this_thread::sleep_for(2000ms);
    }

    return 0;
}