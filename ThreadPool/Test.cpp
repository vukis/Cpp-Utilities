#include "SingleQueueThreadPool.h"
#include "MultiQueueThreadPool.h"
#include "WorkStealingThreadPool.h"
#include "AsioThreadPool.h"
#include "TestUtilities.h"
#ifdef _MSC_VER
#include "PplThreadPool.h"
#endif

void AllocateDeallocateLightWeightData()
{
    const double* tmp = new double[10];
    delete[] tmp;
}

void AllocateDeallocateHeavyData()
{
    const double* tmp = new double[500000];
    delete [] tmp;
}

void SleepForRandomTime()
{
    // Sleeping the thread isn't good as it doesn't tie up the
    // CPU resource in the same way as actual work on a thread would do,
    // The OS is free to schedule work on the CPU while the thread is
    // sleeping. Hence we do some busy work. Note that volatile keyword
    // is necessary to prevent compiler from removing the below code.
    
    srand(0); // random sequences should be indentical
    
    volatile auto delay = rand();
    while (delay != 0) {
        delay--;
    };
}

template<class TaskSystemT>
void TestWithRandomTaskExecutionTime(TaskSystemT&& taskSystem = TaskSystemT{})
{
    const size_t taskCount = 1000;

    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(std::forward<TaskSystemT>(taskSystem).ExecuteAsync(SleepForRandomTime));

    for (auto& result : results)
        result.wait();
}

template<class TaskSystemT>
void TestWithEmptyTask(TaskSystemT&& taskSystem = TaskSystemT{})
{
    const size_t taskCount = 100000;

    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(std::forward<TaskSystemT>(taskSystem).ExecuteAsync([] {}));

    for (auto& result : results)
        result.wait();
}

template<class TaskSystemT>
void TestAllocateDeallocateLightWeightData(TaskSystemT&& taskSystem = TaskSystemT{})
{
    const size_t taskCount = 100000;

    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(std::forward<TaskSystemT>(taskSystem).ExecuteAsync(AllocateDeallocateLightWeightData));

    for (auto& result : results)
        result.wait();
}

template<class TaskSystemT>
void TestAllocateDeallocateHeavyData(TaskSystemT&& taskSystem = TaskSystemT{})
{
    const size_t taskCount = 100000;

    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(std::forward<TaskSystemT>(taskSystem).ExecuteAsync(AllocateDeallocateHeavyData));

    for (auto& result : results)
        result.wait();
}

int main()
{
    constexpr size_t NumOfRuns = 20;

    std::cout << "=========================================" << std::endl;
    std::cout << "Benchmark with random task execution time" << std::endl;
    std::cout << "=========================================" << std::endl;
    FUNCTION_BENCHMARK("Single queue thread pool", NumOfRuns, TestWithRandomTaskExecutionTime<SingleQueueThreadPool>());
    FUNCTION_BENCHMARK("Multi queue thread pool", NumOfRuns, TestWithRandomTaskExecutionTime<MultiQueueThreadPool>());
    FUNCTION_BENCHMARK("Work stealing queue thread pool", NumOfRuns, TestWithRandomTaskExecutionTime<WorkStealingThreadPool>());
    FUNCTION_BENCHMARK("Boost asio based thread pool", NumOfRuns, TestWithRandomTaskExecutionTime<AsioThreadPool>());
#ifdef _MSC_VER
    FUNCTION_BENCHMARK("PPL based thread pool", NumOfRuns, TestWithRandomTaskExecutionTime<PplThreadPool>());
#endif
    std::cout << std::endl;

    std::cout << "=========================================" << std::endl;
    std::cout << "Benchmark with empty task" << std::endl;
    std::cout << "=========================================" << std::endl;
    FUNCTION_BENCHMARK("Single queue thread pool", NumOfRuns, TestWithEmptyTask<SingleQueueThreadPool>());
    FUNCTION_BENCHMARK("Multi queue thread pool", NumOfRuns, TestWithEmptyTask<MultiQueueThreadPool>());
    FUNCTION_BENCHMARK("Work stealing queue thread pool", NumOfRuns, TestWithEmptyTask<WorkStealingThreadPool>());
    FUNCTION_BENCHMARK("Boost asio based thread pool", NumOfRuns, TestWithEmptyTask<AsioThreadPool>());
#ifdef _MSC_VER
    FUNCTION_BENCHMARK("PPL based thread pool", NumOfRuns, TestWithEmptyTask<PplThreadPool>());
#endif
    std::cout << std::endl;

    std::cout << "=========================================" << std::endl;
    std::cout << "Benchmark with light weight data" << std::endl;
    std::cout << "=========================================" << std::endl;
    FUNCTION_BENCHMARK("Single queue thread pool", NumOfRuns, TestAllocateDeallocateLightWeightData<SingleQueueThreadPool>());
    FUNCTION_BENCHMARK("Multi queue thread pool", NumOfRuns, TestAllocateDeallocateLightWeightData<MultiQueueThreadPool>());
    FUNCTION_BENCHMARK("Work stealing queue thread pool", NumOfRuns, TestAllocateDeallocateLightWeightData<WorkStealingThreadPool>());
    FUNCTION_BENCHMARK("Boost asio based thread pool", NumOfRuns, TestAllocateDeallocateLightWeightData<AsioThreadPool>());
#ifdef _MSC_VER
    FUNCTION_BENCHMARK("PPL based thread pool", NumOfRuns, TestAllocateDeallocateLightWeightData<PplThreadPool>());
#endif
    std::cout << std::endl;

    std::cout << "=========================================" << std::endl;
    std::cout << "Benchmark with heavy data" << std::endl;
    std::cout << "=========================================" << std::endl;
    FUNCTION_BENCHMARK("Single queue thread pool", NumOfRuns, TestAllocateDeallocateHeavyData<SingleQueueThreadPool>());
    FUNCTION_BENCHMARK("Multi queue thread pool", NumOfRuns, TestAllocateDeallocateHeavyData<MultiQueueThreadPool>());
    FUNCTION_BENCHMARK("Work stealing queue thread pool", NumOfRuns, TestAllocateDeallocateHeavyData<WorkStealingThreadPool>());
    FUNCTION_BENCHMARK("Boost asio based thread pool", NumOfRuns, TestAllocateDeallocateHeavyData<AsioThreadPool>());
#ifdef _MSC_VER
    FUNCTION_BENCHMARK("PPL based thread pool", NumOfRuns, TestAllocateDeallocateHeavyData<PplThreadPool>());
#endif
    std::cout << std::endl;

    return 0;
}
