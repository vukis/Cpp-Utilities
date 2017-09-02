#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <numeric>

inline void LoadCPUForRandomTime()
{
    // Sleeping the thread isn't good as it doesn't tie up the
    // CPU resource in the same way as actual work on a thread would do,
    // The OS is free to schedule work on the CPU while the thread is
    // sleeping. Hence we do some busy work. Note that volatile keyword
    // is necessary to prevent compiler from removing the below code.

    srand(0); // random sequences should be indentical

    volatile auto delay = rand() % static_cast<int>(1e5);
    while (delay != 0) {
        delay--;
    };
}

template<typename DurationT>
void LoadCPUFor(DurationT&& duration)
{
    // Sleeping the thread isn't good as it doesn't tie up the
    // CPU resource in the same way as actual work on a thread would do,
    // The OS is free to schedule work on the CPU while the thread is
    // sleeping. Hence we do some busy work. Note that volatile keyword
    // is necessary to prevent compiler from removing the below code.

    for (auto start = std::chrono::steady_clock::now(), now = start;
        now < start + duration;
        now = std::chrono::steady_clock::now())
    {
    }
}

template<
    typename TimeT = std::chrono::milliseconds,
    typename ClockT = std::chrono::steady_clock>
    class StopWatch
{
public:

    using TimeRep = TimeT;

    StopWatch()
    {
        Start();
    }

    void Start()
    {
        m_start = m_end = ClockT::now();
    }

    auto Stop()
    {
        m_end = ClockT::now();
        return Elapsed();
    }

    auto Elapsed() const
    {
        auto delta = std::chrono::duration_cast<TimeT>(m_end - m_start);
        m_history.push_back(delta);
        return delta.count();
    }

    auto GetHistory() const
    {
        return m_history;
    }

private:
    std::chrono::time_point<ClockT> m_start;
    std::chrono::time_point<ClockT> m_end;
    mutable std::vector<TimeT>      m_history;
};

using StopWatchMs = StopWatch<>;

#define DO_BENCHMARK_TEST(repeatTimes, test) { \
std::cout << " - Benchmark test ( " << #test; \
StopWatchMs stopWatch; \
for (size_t n = 0; n < repeatTimes; ++n) { \
    stopWatch.Start(); \
    test(); \
    stopWatch.Stop(); \
} \
const auto history = stopWatch.GetHistory(); \
std::cout << " ) => " << (std::accumulate(history.begin(), history.end(), StopWatchMs::TimeRep{}) / repeatTimes).count() << " ms" << std::endl; \
}

#define DO_BENCHMARK_TEST_WITH_DESCRIPTION(description, repeatTimes, test) { \
std::cout << " - Benchmark test ( " << std::setw(40) << std::right << #test << ", description: " << description; \
StopWatchMs stopWatch; \
for (size_t n = 0; n < repeatTimes; ++n) { \
    stopWatch.Start(); \
    test(); \
    stopWatch.Stop(); \
} \
const auto history = stopWatch.GetHistory(); \
std::cout << " ) => " << (std::accumulate(history.begin(), history.end(), StopWatchMs::TimeRep{}) / repeatTimes).count() << " ms" << std::endl; \
}

#define TEST_ASSERT(expr) \
if (!(expr)) { \
    std::ostringstream ss; \
    ss << __FILE__ << ":" <<__LINE__ << " " << #expr; \
    throw std::runtime_error(ss.str()); \
}

#define DO_TEST(test)  { \
std::cout << " - Test ( " << #test; \
try \
{ \
    test(); \
} \
catch (const std::exception& e) \
{ \
    std::cout << " => failed with: " << e.what() << " )" << std::endl; \
    throw; \
} \
std::cout << " => succeed )" << std::endl; \
}
