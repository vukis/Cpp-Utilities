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
    const auto concatenated = ConcatenateStrings(CreateString("Hello"), CreateString("World"));
    TEST_ASSERT("HelloWorld" == concatenated);
}

void Test_ParallelStringConcatenation()
{
    const auto ParallelCreateString(Asynchronize(CreateString));
    const auto ParallelConcatenateStrings(AsyncAdapter(ConcatenateStrings));

    const auto concatenated = ParallelConcatenateStrings(ParallelCreateString("Hello"), ParallelCreateString("World"))().get();
    TEST_ASSERT("HelloWorld" == concatenated);
}

int main()
{

    std::cout << "==========================================" << std::endl;
    std::cout << "             FUNCTIONAL TESTS             " << std::endl;
    std::cout << "==========================================" << std::endl;
    DO_TEST("Test_StringConcatenation", Test_StringConcatenation());
    DO_TEST("Test_ParallelStringConcatenation", Test_ParallelStringConcatenation());
    std::cout << std::endl;

    std::cout << "==========================================" << std::endl;
    std::cout << "            PERFORMANCE TESTS             " << std::endl;
    std::cout << "==========================================" << std::endl;
    constexpr size_t NumOfRuns = 1;
    std::cout << "==========================================" << std::endl;
    std::cout << "      Benchmark string concatenation      " << std::endl;
    std::cout << "==========================================" << std::endl;
    FUNCTION_BENCHMARK("String concatenation", NumOfRuns, Test_StringConcatenation());
    std::cout << std::endl;

    std::cout << "=========================================" << std::endl;
    std::cout << " Benchmark parallel string concatenation  " << std::endl;
    std::cout << "=========================================" << std::endl;
    FUNCTION_BENCHMARK("Parallel string concatenation", NumOfRuns, Test_ParallelStringConcatenation());
    std::cout << std::endl;

    return 0;
}
