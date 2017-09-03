#include "SingleQueueThreadPool.h"
#include "MultiQueueThreadPool.h"
#include "WorkStealingThreadPool.h"
#include "AsioThreadPool.h"
#ifdef _MSC_VER
#include "PplThreadPool.h"
#endif
#include "Common/TestUtilities.h"

template<class TaskSystemT>
void Test_TaskResultIsAsExpected(TaskSystemT&& taskSystem = TaskSystemT{})
{
    constexpr size_t taskCount = 10000;

    std::vector<std::future<size_t>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(taskSystem.ExecuteAsync([i] { return i*i; }));

    for (size_t i = 0; i < taskCount; ++i)
        TEST_ASSERT(i*i == results[i].get());
}

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
void Test_100msTaskExecutionTime(TaskSystemT&& taskSystem = TaskSystemT{})
{
    using namespace std::chrono_literals;

    constexpr size_t taskCount = 10;

    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(taskSystem.ExecuteAsync([] { LoadCPUFor(100ms); }));

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
//    std::cout << "==========================================" << std::endl;
//    std::cout << "             FUNCTIONAL TESTS             " << std::endl;
//    std::cout << "==========================================" << std::endl;
//    DO_TEST(Test_TaskResultIsAsExpected<SingleQueueThreadPool>);
//    DO_TEST(Test_TaskResultIsAsExpected<MultiQueueThreadPool>);
//    DO_TEST(Test_TaskResultIsAsExpected<WorkStealingThreadPool>);
//    DO_TEST(Test_TaskResultIsAsExpected<AsioThreadPool>);
//#ifdef _MSC_VER
//    DO_TEST(Test_TaskResultIsAsExpected<PplThreadPool>);
//#endif
//    std::cout << std::endl;
//
//    std::cout << "==========================================" << std::endl;
//    std::cout << "            PERFORMANCE TESTS             " << std::endl;
//    std::cout << "==========================================" << std::endl;
//    std::cout << "Number of CPUs: " << std::thread::hardware_concurrency() << std::endl;
//    constexpr size_t NumOfRuns = 10;
//    std::cout << std::endl;
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on single task queue ", NumOfRuns, Test_RandomTaskExecutionTime<SingleQueueThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on multiple task queues", NumOfRuns, Test_RandomTaskExecutionTime<MultiQueueThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on work stealing queue ", NumOfRuns, Test_RandomTaskExecutionTime<WorkStealingThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on boost::asio", NumOfRuns, Test_RandomTaskExecutionTime<AsioThreadPool>);
//#ifdef _MSC_VER
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on PPL", NumOfRuns, Test_RandomTaskExecutionTime<PplThreadPool>);
//#endif
//    std::cout << std::endl;
//
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on single task queue ", NumOfRuns, Test_1nsTaskExecutionTime<SingleQueueThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on multiple task queues", NumOfRuns, Test_1nsTaskExecutionTime<MultiQueueThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on work stealing queue ", NumOfRuns, Test_1nsTaskExecutionTime<WorkStealingThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on boost::asio", NumOfRuns, Test_1nsTaskExecutionTime<AsioThreadPool>);
//#ifdef _MSC_VER
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on PPL", NumOfRuns, Test_1nsTaskExecutionTime<PplThreadPool>);
//#endif
//    std::cout << std::endl;
//
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on single task queue ", NumOfRuns, Test_100msTaskExecutionTime<SingleQueueThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on multiple task queues", NumOfRuns, Test_100msTaskExecutionTime<MultiQueueThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on work stealing queue ", NumOfRuns, Test_100msTaskExecutionTime<WorkStealingThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on boost::asio", NumOfRuns, Test_100msTaskExecutionTime<AsioThreadPool>);
//#ifdef _MSC_VER
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on PPL", NumOfRuns, Test_100msTaskExecutionTime<PplThreadPool>);
//#endif
//    std::cout << std::endl;
//
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on single task queue ", NumOfRuns, Test_EmptyTask<SingleQueueThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on multiple task queues", NumOfRuns, Test_EmptyTask<MultiQueueThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on work stealing queue ", NumOfRuns, Test_EmptyTask<WorkStealingThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on boost::asio", NumOfRuns, Test_EmptyTask<AsioThreadPool>);
//#ifdef _MSC_VER
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on PPL", NumOfRuns, Test_EmptyTask<PplThreadPool>);
//#endif
//    std::cout << std::endl;
//
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on single task queue ", NumOfRuns, Test_MultipleTaskProducers<SingleQueueThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on multiple task queues", NumOfRuns, Test_MultipleTaskProducers<MultiQueueThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on work stealing queue ", NumOfRuns, Test_MultipleTaskProducers<WorkStealingThreadPool>);
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on boost::asio", NumOfRuns, Test_MultipleTaskProducers<AsioThreadPool>);
//#ifdef _MSC_VER
//    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on PPL", NumOfRuns, Test_MultipleTaskProducers<PplThreadPool>);
//#endif
//    std::cout << std::endl;
// 
    return 0;
}
