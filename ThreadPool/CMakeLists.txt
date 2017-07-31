cmake_minimum_required(VERSION 3.2)

set(EXECUTABLE_NAME ThreadPool-Test)

# Test ThreadPool
set(TEST_FILES Test.cpp)

# Dependencies
if(NOT MSVC)
    find_package(Threads REQUIRED)
endif()
## Boost
find_package(Boost 1.64.0 REQUIRED COMPONENTS) #system filesystem
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)

# Include header files directories
include_directories(${Boost_INCLUDE_DIRS})
# Static libraries directories
link_directories(${Boost_LIBRARY_DIRS})

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