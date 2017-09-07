#include "TestUtilities.h"
#include "Asynchronize.h"
#include "FixedFunction.h"
#include "ZipIterartor.h"
#include <string>

inline std::string CreateString(const char* str)
{
    using namespace std::chrono_literals;
    LoadCPUFor(1s);
    return str;
}

inline auto ConcatenateStrings(const std::string& str1, const std::string& str2)
{
    using namespace std::chrono_literals;
    LoadCPUFor(1s);
    return str1 + str2;
}

void Test_StringConcatenation()
{
    const auto concatenated = ConcatenateStrings(
        ConcatenateStrings(CreateString("Hello"), CreateString(" ")),
        ConcatenateStrings(CreateString("World"), CreateString("!")));

    TEST_ASSERT("Hello World!" == concatenated);
}

void Test_ParallelStringConcatenation_UsingStdAsyncStaightforward()
{
    auto createHello{ std::async(std::launch::async, CreateString, "Hello") };
    auto createSpace{ std::async(std::launch::async, CreateString, " ") };
    auto createFirstPart{ std::async(std::launch::async, ConcatenateStrings, createHello.get(), createSpace.get()) };

    auto createWorld{ std::async(std::launch::async, CreateString, "World") };
    auto createExclamationPoint{ std::async(std::launch::async, CreateString, "!") };
    auto createSecondPart{ std::async(std::launch::async, ConcatenateStrings, createWorld.get(), createExclamationPoint.get()) };

    auto concatenate{ std::async(std::launch::async, ConcatenateStrings, createFirstPart.get(), createSecondPart.get()) };

    const auto concatenated = concatenate.get();
    TEST_ASSERT("Hello World!" == concatenated);
}

void Test_ParallelStringConcatenation()
{
    const auto ParallelCreateString{ Asynchronize(CreateString) };
    const auto ParallelConcatenateStrings{ AsyncAdapter(ConcatenateStrings) };

    const auto createHello{ ParallelCreateString("Hello") };
    const auto createSpace{ ParallelCreateString(" ") };
    const auto createFirstPart{ ParallelConcatenateStrings(createHello, createSpace) };

    const auto createWorld{ ParallelCreateString("World") };
    const auto createExclamationPoint{ ParallelCreateString("!") };
    const auto createSecondPart{ ParallelConcatenateStrings(createWorld, createExclamationPoint) };

    const auto concatenated = ParallelConcatenateStrings(createFirstPart, createSecondPart)().get();
    TEST_ASSERT("Hello World!" == concatenated);
}

void Test_FixedFunctionResultIsAsExpected()
{
    FixedFunction<int()> function{ [] { return 42; } };
    TEST_ASSERT(42 == function());
}

void Test_FixedFunctionWithMoveOlnlyTypes()
{
    struct Test {
        explicit Test(int value) : m_pValue{ std::make_unique<int>(value) } {}
        auto operator()() { return *m_pValue; }
    private:
        std::unique_ptr<int> m_pValue;
    };

    // std::function<int()> function = Test{ 42 }; // doesn't compile since Test is non-copyable 
    // (because it contains move-only type std::unique_ptr).
    FixedFunction<int()> function = Test{ 42 };
    TEST_ASSERT(42 == function());
}

void Test_FixedFunctionWithCpuTimeRequiringTask()
{
    FixedFunction<int()> function{ [] { LoadCPUFor(std::chrono::milliseconds(1000)); return 42; } };
    TEST_ASSERT(42 == function());
}

void Test_StdFunctionWithCpuTimeRequiringTask()
{
    std::function<int()> function{ [] { LoadCPUFor(std::chrono::milliseconds(1000)); return 42; } };
    TEST_ASSERT(42 == function());
}

void Test_CreateZip()
{
    const std::vector<char> input1 = { 'A', 'B', 'C', 'D', 'E' };
    const std::vector<int> input2 = { 1, 2, 3, 4, 5 };
    //std::vector<std::tuple<char, int>> expected = { std::tuple<char, int>"A1", "B2", "C3", "D4", "E5" };
    //std::vector<std::string> result;

    const auto zipped = MakeZipContainer(input1, input2);

    // TEST_ASSERT(result == expected);
}

void Test_StlSortZip()
{

}

void Test_StlAccumulateZip()
{

}

void Test_StlTransfromZip()
{

}

int main()
{
    std::cout << "==========================================" << std::endl;
    std::cout << "             FUNCTIONAL TESTS             " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "=           Test FixedFunction           =" << std::endl;
    DO_TEST(Test_FixedFunctionResultIsAsExpected);
    DO_TEST(Test_FixedFunctionWithMoveOlnlyTypes);
    std::cout << std::endl;

    std::cout << "=           Test Asyncronize             =" << std::endl;
    DO_TEST(Test_StringConcatenation);
    DO_TEST(Test_ParallelStringConcatenation_UsingStdAsyncStaightforward);
    DO_TEST(Test_ParallelStringConcatenation);
    std::cout << std::endl;

    std::cout << "=          Test Zip container            =" << std::endl;
    DO_TEST(Test_CreateZip);
    std::cout << std::endl;

    std::cout << "==========================================" << std::endl;
    std::cout << "            PERFORMANCE TESTS             " << std::endl;
    std::cout << "==========================================" << std::endl;
    constexpr size_t NumOfRuns = 10;
    std::cout << "= Test FixedFunction against std::function =" << std::endl;
    DO_BENCHMARK_TEST(NumOfRuns, Test_FixedFunctionWithCpuTimeRequiringTask);
    DO_BENCHMARK_TEST(NumOfRuns, Test_StdFunctionWithCpuTimeRequiringTask);
    std::cout << std::endl;

    std::cout << "=           Test Asyncronize             =" << std::endl;
    DO_BENCHMARK_TEST(1, Test_StringConcatenation);
    DO_BENCHMARK_TEST(1, Test_ParallelStringConcatenation_UsingStdAsyncStaightforward);
    DO_BENCHMARK_TEST(1, Test_ParallelStringConcatenation);
    std::cout << std::endl;

    return 0;
}
