#pragma once

#include "TaskQueue.h"
#include <algorithm>
#include <thread>

class WorkStealingQueueThreadPool
{
public:

    explicit WorkStealingQueueThreadPool(size_t threadCount = std::max(1u, std::thread::hardware_concurrency()));
    ~WorkStealingQueueThreadPool();

    template<typename TaskT>
    auto ExecuteAsync(TaskT&& task)
    {
        const auto index = m_queueIndex++;
        for (size_t n = 0; n != m_queues.size()*m_tryoutCount; ++n)
        {
            auto result = m_queues[(index + n) % m_queues.size()].TryPush(std::forward<TaskT>(task));

            if (result.has_value())
                return std::move(*result);
        }
        return m_queues[index % m_queues.size()].Push(std::forward<TaskT>(task));
    }

private:

    void Run(size_t queueIndex);

    std::vector<TaskQueue> m_queues;
    std::atomic<size_t>    m_queueIndex{ 0 };
    const size_t m_tryoutCount{ 1 };

    std::vector<std::thread> m_threads;
};

WorkStealingQueueThreadPool::WorkStealingQueueThreadPool(size_t threadCount)
    : m_queues{ threadCount }
{
    for (size_t index = 0; index != threadCount; ++index)
        m_threads.emplace_back([this, index] { Run(index); });
}

WorkStealingQueueThreadPool::~WorkStealingQueueThreadPool()
{
    for (auto& queue : m_queues)
        queue.SetEnabled(false);

    for (auto& thread : m_threads)
        thread.join();
}

void WorkStealingQueueThreadPool::Run(size_t queueIndex)
{
    while (m_queues[queueIndex].IsEnabled())
    {
        TaskQueue::TaskType task;
        for (size_t n = 0; n != m_queues.size()*m_tryoutCount; ++n)
        {
            if (m_queues[(queueIndex + n) % m_queues.size()].TryPop(task))
                break;
        }

        if (!task && !m_queues[queueIndex].WaitAndPop(task))
            return;

        task();
    }
}