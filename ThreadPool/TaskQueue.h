#pragma once

#include <condition_variable>
#include <optional>
#include <functional>
#include <future>
#include <queue>

class TaskBase
{
public:
    virtual ~TaskBase() = default;
    virtual void exec() = 0;
    void operator()() { exec(); }
};

template <typename T> 
class Task : public TaskBase
{
public:
    Task(T &&t) : task(std::move(t)) {}
    void exec() override { task(); }

    T task;
};

class TaskQueue
{
public:
    using TaskPtrType = std::unique_ptr<TaskBase>;

    TaskQueue() = default;
    ~TaskQueue() = default;

    TaskQueue(TaskQueue &&) = default;
    TaskQueue &operator=(TaskQueue &&) = default;

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

    auto WaitAndPop(TaskPtrType &task) 
    {
        LockType lock{ m_mutex };
        m_ready.wait(lock, [this] { return !m_enabled || !m_queue.empty(); });
        if (m_enabled && !m_queue.empty()) {
            task = std::move(m_queue.front());
            m_queue.pop();
            return true;
        }
        return false;
    }

    template <typename TaskT>
    auto Push(TaskT &&task) // -> std::future<decltype(task())>
    {
        using TaskRetType = decltype(task());
        using PkgTask = std::packaged_task<TaskRetType()>;
        auto job = 
            std::make_unique<Task<PkgTask>>(PkgTask(std::forward<TaskT>(task)));
        auto future = job->task.get_future();

        {
            LockType lock{ m_mutex };
            m_queue.emplace(std::move(job));
        }

        m_ready.notify_one();
        return future;
    }

    auto TryPop(TaskPtrType &task) 
    {
        LockType lock{ m_mutex, std::try_to_lock };

        if (!lock || m_enabled || m_queue.empty())
            return false;

        task = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    template <typename TaskT>
    auto TryPush(TaskT &&task) // -> std::optional<std::future<decltype(task())>>
    {
        using TaskRetType = decltype(task());

        std::optional<std::future<TaskRetType>> future;
        {
            LockType lock{ m_mutex, std::try_to_lock };
            if (!lock)
                return future;

            using PkgTask = std::packaged_task<TaskRetType()>;
            auto job =
                std::make_unique<Task<PkgTask>>(PkgTask(std::forward<TaskT>(task)));
            future = job->task.get_future();
            m_queue.emplace(std::move(job));
        }

        m_ready.notify_one();
        return future;
    }

private:
    TaskQueue(const TaskQueue &) = delete;
    TaskQueue &operator=(const TaskQueue &) = delete;

    using LockType = std::unique_lock<std::mutex>;

    std::queue<TaskPtrType> m_queue;
    bool m_enabled{ true };
    mutable std::mutex m_mutex;
    std::condition_variable m_ready;
};