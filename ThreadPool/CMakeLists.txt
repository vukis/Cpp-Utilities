cmake_minimum_required(VERSION 3.2)

set(EXECUTABLE_NAME ThreadPool-Test)

# Test ThreadPool
set(TEST_FILES Test.cpp)

# Create test executable
add_executable(${EXECUTABLE_NAME} ${TEST_FILES})
## Linking up all libraries
target_link_libraries(${EXECUTABLE_NAME} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
if(NOT MSVC)
	target_link_libraries(${EXECUTABLE_NAME} pthread)
endif()

add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})

find_package(codecov)
add_coverage(${EXECUTABLE_NAME})