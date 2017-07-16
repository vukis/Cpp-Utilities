#pragma once

#include <deque>
#include <optional>
#include <functional>
#include <condition_variable>
#include <future>

class TaskQueue
{
public:

    using TaskType = std::function<void()>;

    TaskQueue() = default;
    ~TaskQueue() = default;

    TaskQueue(TaskQueue&&) = default;
    TaskQueue& operator=(TaskQueue&&) = default;

    void SetDone(bool done)
    {
        {
            LockType lock{ m_mutex };
            m_done = done;
        }

        if (done)
            m_ready.notify_all();
    }

    auto IsDone() const
    {
        LockType lock{ m_mutex };
        return m_done;
    }

    auto WaitAndPop(TaskType& task)
    {
        LockType lock{ m_mutex };

        while (m_queue.empty() && !m_done)
            m_ready.wait(lock);

        if (!m_queue.empty() && !m_done)
        {
            task = std::move(m_queue.front());
            m_queue.pop_front();
            return true;
        }

        return false;
    }

    template<typename TTask>
    auto Push(TTask&& task) // -> std::future<decltype(task())>
    {
        using TaskReturnType = decltype(task());

        // std::packaged_task<> is move only type.
        // We need to wrap it in a shared_ptr:
        auto packagedTask = std::make_shared<std::packaged_task<TaskReturnType()>>(std::forward<TTask>(task));
        auto future = packagedTask->get_future();

        {
            LockType lock{ m_mutex };
            m_queue.emplace_back([packagedTask] { (*packagedTask)(); });
        }

        m_ready.notify_one();
        return future;
    }

    auto TryPop(TaskType& task)
    {
        LockType lock{ m_mutex, std::try_to_lock };

        if (!lock || m_queue.empty())
            return false;

        task = move(m_queue.front());
        m_queue.pop_front();
        return true;
    }

    template<typename TTask>
    auto TryPush(TTask&& task) -> std::optional<std::future<decltype(task())>>
    {
        using TaskReturnType = decltype(task());

        // std::packaged_task<void()> is not movable
        // We need to wrap it in a shared_ptr:
        auto packagedTask = std::make_shared<std::packaged_task<TaskReturnType()>>(std::forward<TTask>(task));
        auto future = packagedTask->get_future();

        {
            LockType lock{ m_mutex, std::try_to_lock };

            if (!lock)
                return {};

            m_queue.emplace_back([packagedTask]() { (*packagedTask)(); });
        }

        m_ready.notify_one();
        return future;
    }

private:

    using LockType = std::unique_lock<std::mutex>;

    std::deque<TaskType> m_queue;
    bool                 m_done{ false };
    mutable std::mutex      m_mutex;
    std::condition_variable m_ready;

    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;
};