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

    void SetEnabled(bool enabled)
    {
        {
            LockType lock{ m_mutex };
            m_enabled = enabled;
        }

        if (!enabled)
            m_ready.notify_all();
    }

    auto IsEnabled() const
    {
        LockType lock{ m_mutex };
        return m_enabled;
    }

    auto WaitAndPop(TaskType& task)
    {
        LockType lock{ m_mutex };

        m_ready.wait(lock, [this] { return !m_enabled || !m_queue.empty(); });

        if (m_enabled && !m_queue.empty())
        {
            task = std::move(m_queue.front());
            m_queue.pop_front();
            return true;
        }

        return false;
    }

    template<typename TaskT>
    auto Push(TaskT&& task) // -> std::future<decltype(task())>
    {
        using TaskReturnType = decltype(task());

        // std::packaged_task<> is move only type.
        // We need to wrap it in a shared_ptr:
        auto packagedTask = std::make_shared<std::packaged_task<TaskReturnType()>>(std::forward<TaskT>(task));
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

        if (!lock || m_enabled || m_queue.empty())
            return false;

        task = std::move(m_queue.front());
        m_queue.pop_front();
        return true;
    }

    template<typename TaskT>
    auto TryPush(TaskT&& task) -> std::optional<std::future<decltype(task())>>
    {
        using TaskReturnType = decltype(task());

        std::future<TaskReturnType> future;
        {
            LockType lock{ m_mutex, std::try_to_lock };

            if (!lock)
                return {};

            // std::packaged_task<void()> is not movable
            // We need to wrap it in a shared_ptr:
            auto packagedTask = std::make_shared<std::packaged_task<TaskReturnType()>>(std::forward<TaskT>(task));
            future = packagedTask->get_future();

            m_queue.emplace_back([packagedTask]() { (*packagedTask)(); });
        }

        m_ready.notify_one();
        return future;
    }

private:

    using LockType = std::unique_lock<std::mutex>;

    std::deque<TaskType> m_queue;
    bool                 m_enabled{ true };
    mutable std::mutex      m_mutex;
    std::condition_variable m_ready;

    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;
};