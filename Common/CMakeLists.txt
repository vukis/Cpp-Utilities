cmake_minimum_required(VERSION 3.2)

# Dependencies
if(NOT MSVC)
    find_package(Threads REQUIRED)
endif()

# Create executable
set(EXECUTABLE_NAME TestCommon)
set(TEST_FILES Test.cpp)
add_executable(${EXECUTABLE_NAME} ${TEST_FILES})

if(NOT MSVC)
    target_link_libraries(${EXECUTABLE_NAME} pthread)
endif()

add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})

# Code coverage
find_package(codecov)
add_coverage(${EXECUTABLE_NAME})
