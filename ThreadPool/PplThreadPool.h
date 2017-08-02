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
        using TaskRetType = decltype(task());
        using PkgTask = std::packaged_task<TaskRetType()>;

        // std::packaged_task<> is move only type.
        // We need to wrap it in a shared_ptr to use in lambda.
        auto job =
            std::make_shared<PkgTask>(std::forward<TaskT>(task));
        auto future = job->get_future();

        m_tasks.run([job = std::move(job)] { (*job)(); });

        return future;
    }

private:

    concurrency::task_group m_tasks;
};

PplThreadPool::~PplThreadPool()
{
    m_tasks.wait();
}
#endif