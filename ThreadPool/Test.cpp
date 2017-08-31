#include "SingleQueueThreadPool.h"
#include "MultiQueueThreadPool.h"
#include "WorkStealingThreadPool.h"
#include "AsioThreadPool.h"
#ifdef _MSC_VER
#include "PplThreadPool.h"
#endif
#include "Common/TestUtilities.h"

template<class TaskSystemT>
void Test_RandomTaskExecutionTime(TaskSystemT&& taskSystem = TaskSystemT{})
{
    constexpr size_t taskCount = 10000;

    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(taskSystem.ExecuteAsync(LoadCPUForRandomTime));

    for (auto& result : results)
        result.wait();
}

template<class TaskSystemT>
void Test_1nsTaskExecutionTime(TaskSystemT&& taskSystem = TaskSystemT{})
{
    using namespace std::chrono_literals;

    constexpr size_t taskCount = 10000;

    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(taskSystem.ExecuteAsync([] { LoadCPUFor(1ns); }));

    for (auto& result : results)
        result.wait();
}

template<class TaskSystemT>
void Test_10msTaskExecutionTime(TaskSystemT&& taskSystem = TaskSystemT{})
{
    using namespace std::chrono_literals;

    constexpr size_t taskCount = 10;

    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(taskSystem.ExecuteAsync([] { LoadCPUFor(10ms); }));

    for (auto& result : results)
        result.wait();
}

template<class TaskSystemT>
void Test_EmptyTask(TaskSystemT&& taskSystem = TaskSystemT{})
{
    constexpr size_t taskCount = 10000;

    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(taskSystem.ExecuteAsync([] {}));

    for (auto& result : results)
        result.wait();
}

template<class TaskSystemT, class TaskT>
void RepeatTask(TaskSystemT&& taskSystem, TaskT&& task, size_t times)
{
    std::vector<std::future<void>> results;

    // Here we need not to std::forward just copy task.
    // Because if the universal reference of task has bound to an r-value reference 
    // then std::forward will have the same effect as std::move and thus task is not required to contain a valid task. 
    // Universal reference must only be std::forward'ed a exactly zero or one times.
    for (size_t i = 0; i < times; ++i)
        results.push_back(taskSystem.ExecuteAsync(task));

    for (auto& result : results)
        result.wait();
}

template<class TaskSystemT>
void Test_MultipleTaskProducers(TaskSystemT&& taskSystem = TaskSystemT{})
{
    constexpr size_t taskCount = 10000;

    std::vector<std::thread> taskProducers{ std::max(1u, std::thread::hardware_concurrency()) };

    for (auto& producer : taskProducers)
        producer = std::thread([&] { RepeatTask(taskSystem, &LoadCPUForRandomTime, taskCount); });

    for (auto& producer : taskProducers)
    {
        if (producer.joinable())
            producer.join();
    }
}

int main()
{
    constexpr size_t NumOfRuns = 10;

    std::cout << "Number of CPUs: " << std::thread::hardware_concurrency() << std::endl;

    std::cout << "=========================================" << std::endl;
    std::cout << "Benchmark with random task execution time" << std::endl;
    std::cout << "=========================================" << std::endl;
    FUNCTION_BENCHMARK("Single queue thread pool", NumOfRuns, Test_RandomTaskExecutionTime<SingleQueueThreadPool>());
    FUNCTION_BENCHMARK("Multi queue thread pool", NumOfRuns, Test_RandomTaskExecutionTime<MultiQueueThreadPool>());
    FUNCTION_BENCHMARK("Work stealing queue thread pool", NumOfRuns, Test_RandomTaskExecutionTime<WorkStealingThreadPool>());
    FUNCTION_BENCHMARK("Boost asio based thread pool", NumOfRuns, Test_RandomTaskExecutionTime<AsioThreadPool>());
#ifdef _MSC_VER
    FUNCTION_BENCHMARK("PPL based thread pool", NumOfRuns, Test_RandomTaskExecutionTime<PplThreadPool>());
#endif
    std::cout << std::endl;

    std::cout << "=========================================" << std::endl;
    std::cout << "Benchmark with 1 ns task execution time" << std::endl;
    std::cout << "=========================================" << std::endl;
    FUNCTION_BENCHMARK("Single queue thread pool", NumOfRuns, Test_1nsTaskExecutionTime<SingleQueueThreadPool>());
    FUNCTION_BENCHMARK("Multi queue thread pool", NumOfRuns, Test_1nsTaskExecutionTime<MultiQueueThreadPool>());
    FUNCTION_BENCHMARK("Work stealing queue thread pool", NumOfRuns, Test_1nsTaskExecutionTime<WorkStealingThreadPool>());
    FUNCTION_BENCHMARK("Boost asio based thread pool", NumOfRuns, Test_1nsTaskExecutionTime<AsioThreadPool>());
#ifdef _MSC_VER
    FUNCTION_BENCHMARK("PPL based thread pool", NumOfRuns, Test_1nsTaskExecutionTime<PplThreadPool>());
#endif
    std::cout << std::endl;

    std::cout << "=========================================" << std::endl;
    std::cout << "Benchmark with 10 ms task execution time" << std::endl;
    std::cout << "=========================================" << std::endl;
    FUNCTION_BENCHMARK("Single queue thread pool", NumOfRuns, Test_10msTaskExecutionTime<SingleQueueThreadPool>());
    FUNCTION_BENCHMARK("Multi queue thread pool", NumOfRuns, Test_10msTaskExecutionTime<MultiQueueThreadPool>());
    FUNCTION_BENCHMARK("Work stealing queue thread pool", NumOfRuns, Test_10msTaskExecutionTime<WorkStealingThreadPool>());
    FUNCTION_BENCHMARK("Boost asio based thread pool", NumOfRuns, Test_10msTaskExecutionTime<AsioThreadPool>());
#ifdef _MSC_VER
    FUNCTION_BENCHMARK("PPL based thread pool", NumOfRuns, Test_10msTaskExecutionTime<PplThreadPool>());
#endif
    std::cout << std::endl;

    std::cout << "=========================================" << std::endl;
    std::cout << "Benchmark with empty task" << std::endl;
    std::cout << "=========================================" << std::endl;
    FUNCTION_BENCHMARK("Single queue thread pool", NumOfRuns, Test_EmptyTask<SingleQueueThreadPool>());
    FUNCTION_BENCHMARK("Multi queue thread pool", NumOfRuns, Test_EmptyTask<MultiQueueThreadPool>());
    FUNCTION_BENCHMARK("Work stealing queue thread pool", NumOfRuns, Test_EmptyTask<WorkStealingThreadPool>());
    FUNCTION_BENCHMARK("Boost asio based thread pool", NumOfRuns, Test_EmptyTask<AsioThreadPool>());
#ifdef _MSC_VER
    FUNCTION_BENCHMARK("PPL based thread pool", NumOfRuns, Test_EmptyTask<PplThreadPool>());
#endif
    std::cout << std::endl;

    std::cout << "=====================================================================" << std::endl;
    std::cout << "Benchmark with multiple task producers and random task execution time" << std::endl;
    std::cout << "=====================================================================" << std::endl;
    FUNCTION_BENCHMARK("Single queue thread pool", NumOfRuns, Test_MultipleTaskProducers<SingleQueueThreadPool>());
    FUNCTION_BENCHMARK("Multi queue thread pool", NumOfRuns, Test_MultipleTaskProducers<MultiQueueThreadPool>());
    FUNCTION_BENCHMARK("Work stealing queue thread pool", NumOfRuns, Test_MultipleTaskProducers<WorkStealingThreadPool>());
    FUNCTION_BENCHMARK("Boost asio based thread pool", NumOfRuns, Test_MultipleTaskProducers<AsioThreadPool>());
#ifdef _MSC_VER
    FUNCTION_BENCHMARK("PPL based thread pool", NumOfRuns, Test_MultipleTaskProducers<PplThreadPool>());
#endif
    std::cout << std::endl;

    return 0;
}
