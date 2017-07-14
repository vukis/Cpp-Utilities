#pragma once

#include "TaskQueue.h"
#include <algorithm>
#include <thread>

class WorkStealingQueueThreadPool
{
public:

    explicit WorkStealingQueueThreadPool(size_t threadCount = std::max(2u, std::thread::hardware_concurrency()));
    ~WorkStealingQueueThreadPool();

    template<typename TTask>
    auto ExecuteAsync(TTask&& task)
    {
        const auto index = m_queueIndex++;
        for (size_t n = 0; n != m_queueCount*m_tryoutCount; ++n)
        {
            auto result = m_queues[(index + n) % m_queueCount].TryPush(std::forward<TTask>(task));

            if (result.has_value())
                return std::move(*result);
        }
        return m_queues[index % m_queueCount].Push(std::forward<TTask>(task));
    }

private:

    void Run(size_t queueIndex);

    std::vector<TaskQueue> m_queues;
    size_t       m_queueIndex{ 0 };
    const size_t m_queueCount;
    const size_t m_tryoutCount{ 1 };

    std::vector<std::thread> m_threads;
};

WorkStealingQueueThreadPool::WorkStealingQueueThreadPool(size_t threadCount)
    : m_queues{ threadCount }
    , m_queueCount{ threadCount }
{
    for (size_t index = 0; index != threadCount; ++index)
        m_threads.emplace_back([this, index] { Run(index); });
}

WorkStealingQueueThreadPool::~WorkStealingQueueThreadPool()
{
    for (auto& queue : m_queues)
        queue.SetDone(true); // TODO: tagged_bool

    for (auto& thread : m_threads)
        thread.join();
}

void WorkStealingQueueThreadPool::Run(size_t queueIndex)
{
    while (!m_queues[queueIndex].IsDone())
    {
        TaskQueue::TaskType task;
        for (size_t n = 0; n != m_queueCount*m_tryoutCount; ++n)
        {
            if (m_queues[(queueIndex + n) % m_queueCount].TryPop(task))
                break;
        }

        if (!task && !m_queues[queueIndex].WaitAndPop(task))
            return;

        task();
    }
}