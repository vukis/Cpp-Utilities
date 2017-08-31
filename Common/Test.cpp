#include "Asynchronize.h"
#include "TestUtilities.h"
#include <string>

inline std::string CreateString(const char* str)
{
    using namespace std::chrono_literals;
    LoadCPUFor(1s);
    return str;
}

template<typename StringT = std::string>
inline auto ConcatenateStrings(const StringT& str1, const StringT& str2)
{
    using namespace std::chrono_literals;
    LoadCPUFor(1s);
    return str1 + str2;
}

void Test_StringConcatenation()
{
    ConcatenateStrings(CreateString("Hello"), CreateString("World"));
}

void Test_ParallelStringConcatenation()
{
    const auto ParallelCreateString(Asynchronize(CreateString));
    const auto ParallelConcatenateStrings(AsyncAdapter(ConcatenateStrings<std::string>));

    ParallelConcatenateStrings(ParallelCreateString("Hello"), ParallelCreateString("World"))().get();
}

int main()
{
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
