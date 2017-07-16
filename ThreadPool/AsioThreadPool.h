#pragma once

#include <boost/asio.hpp>

#include <functional>
#include <future>
#include <vector>
#include <memory>

class AsioThreadPool
{
public:
    explicit AsioThreadPool(size_t threadCount = std::max(2u, std::thread::hardware_concurrency()));
    ~AsioThreadPool();

    template <typename TaskT>
    auto ExecuteAsync(TaskT&& task) // TODO: Rename: Post
    {
        using TaskReturnType = decltype(task());

        // std::packaged_task<> is move only type.
        // We need to wrap it in a shared_ptr:
        auto packagedTask = std::make_shared<std::packaged_task<TaskReturnType()>>(std::forward<TaskT>(task));
        auto future = packagedTask->get_future();

        m_ioService.post([packagedTask] { (*packagedTask)(); });
        return future;
    }

private:
    void Start();
    void Stop();
    void Run();

    boost::asio::io_service m_ioService;
    std::unique_ptr<boost::asio::io_service::work> m_work{ std::make_unique<boost::asio::io_service::work>(m_ioService) };
    std::vector<std::thread> m_threads;

    AsioThreadPool(const AsioThreadPool&) = delete;
    AsioThreadPool& operator=(const AsioThreadPool&) = delete;
};

AsioThreadPool::AsioThreadPool(size_t threadCount)
    : m_threads(threadCount)
{
    Start();
}

AsioThreadPool::~AsioThreadPool()
{
    Stop();
}

void AsioThreadPool::Start()
{
    for (auto& thread : m_threads)
        thread = std::thread(&AsioThreadPool::Run, this);
}

void AsioThreadPool::Stop()
{
    m_work.reset();
    m_ioService.stop();

    for (auto& thread : m_threads)
    {
        if (thread.joinable())
            thread.join();
    }
}

void AsioThreadPool::Run()
{
    m_ioService.run();
}