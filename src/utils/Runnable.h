#pragma once

#include <atomic>
#include <functional>
#include <iostream>
#include <thread>

using namespace std::placeholders;
using namespace std::chrono_literals;

namespace vendor {
namespace nlab {
namespace utils {

class IRunnable
{
public:
    using Routine = std::function<void(const std::atomic_bool& request_to_stop)>;

    virtual ~IRunnable() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
};

class Runnable : IRunnable
{
public:
    Runnable(Routine routine)
        : m_routine{routine}
        , m_request_to_stop{false}
    {
    }

    virtual ~Runnable() { stop(); }

    void start() override { m_thread = std::thread(m_routine, std::ref(m_request_to_stop)); }

    void stop() override
    {
        m_request_to_stop = true;
        if (m_thread.joinable()) m_thread.join();
    }

private:
    Routine m_routine;
    std::thread m_thread;
    std::atomic_bool m_request_to_stop;
};

} // namespace utils
} // namespace nlab
} // namespace vendor