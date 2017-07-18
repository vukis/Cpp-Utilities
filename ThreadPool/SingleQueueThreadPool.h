#pragma once

#include "TaskQueue.h"
#include <algorithm>
#include <thread>

class SingleQueueThreadPool
{
public:

    explicit SingleQueueThreadPool(size_t threadCount = std::max(2u, std::thread::hardware_concurrency()));
    ~SingleQueueThreadPool();

    template<typename TaskT>
    auto ExecuteAsync(TaskT&& task)
    {
        return m_queue.Push(std::forward<TaskT>(task));
    }

private:
    void Run();

    TaskQueue m_queue;
    std::vector<std::thread> m_threads;
};

SingleQueueThreadPool::SingleQueueThreadPool(size_t threadCount)
{
    for (size_t n = 0; n != threadCount; ++n)
        m_threads.emplace_back([this] { Run(); });
}

SingleQueueThreadPool::~SingleQueueThreadPool()
{
    m_queue.SetEnabled(false);

    for (auto& thread : m_threads)
        thread.join();
}

void SingleQueueThreadPool::Run()
{
    while (m_queue.IsEnabled())
    {
        TaskQueue::TaskType task;
        if (m_queue.WaitAndPop(task))
            task();
    }
}
