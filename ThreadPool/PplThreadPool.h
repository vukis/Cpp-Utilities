#pragma once
#ifdef _MSC_VER
#include <ppltasks.h>
#include <future>
#include <ppl.h>
#include <agents.h>

class PplThreadPool
{
public:

    PplThreadPool() = default;
    ~PplThreadPool();

    template<typename TaskT>
    auto ExecuteAsync(TaskT&& task)
    {
        using TaskReturnType = decltype(task());

        // std::packaged_task<> is move only type.
        // We need to wrap it in a shared_ptr:
        auto packagedTask = std::make_shared<std::packaged_task<TaskReturnType()>>(std::forward<TaskT>(task));
        auto future = packagedTask->get_future();

        m_tasks.emplace_back([packagedTask] { (*packagedTask)(); });

        return future;
    }

private:

    std::vector<concurrency::task<void>> m_tasks;
};

PplThreadPool::~PplThreadPool()
{
    concurrency::when_all(m_tasks.begin(), m_tasks.end()).wait();
}
#endif