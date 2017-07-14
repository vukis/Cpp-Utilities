#pragma once

#include <string>
#include <chrono>
#include <iostream>
#include <numeric>

template<
    typename TimeT = std::chrono::milliseconds,
    typename ClockT = std::chrono::high_resolution_clock>
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

#define FUNCTION_BENCHMARK(name, repeatTimes, function) { \
StopWatchMs stopWatch; \
for (size_t n = 0; n < repeatTimes; ++n) { \
    stopWatch.Start(); \
    function; \
    stopWatch.Stop(); \
} \
const auto history = stopWatch.GetHistory(); \
std::cout << name << ": " << (std::accumulate(history.begin(), history.end(), StopWatchMs::TimeRep{}) / repeatTimes).count() << " ms" << std::endl; \
}
