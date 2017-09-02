#include "TestUtilities.h"
#include "Asynchronize.h"
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

int main()
{
    std::cout << "==========================================" << std::endl;
    std::cout << "             FUNCTIONAL TESTS             " << std::endl;
    std::cout << "==========================================" << std::endl;
    DO_TEST(Test_StringConcatenation);
    DO_TEST(Test_ParallelStringConcatenation_UsingStdAsyncStaightforward);
    DO_TEST(Test_ParallelStringConcatenation);
    std::cout << std::endl;

    std::cout << "==========================================" << std::endl;
    std::cout << "            PERFORMANCE TESTS             " << std::endl;
    std::cout << "==========================================" << std::endl;
    constexpr size_t NumOfRuns = 1;
    DO_BENCHMARK_TEST(NumOfRuns, Test_StringConcatenation);
    DO_BENCHMARK_TEST( NumOfRuns, Test_ParallelStringConcatenation_UsingStdAsyncStaightforward);
    DO_BENCHMARK_TEST(NumOfRuns, Test_ParallelStringConcatenation);

    return 0;
}
