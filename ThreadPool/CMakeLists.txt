cmake_minimum_required(VERSION 3.2)

# Dependencies
if(NOT MSVC)
    find_package(Threads REQUIRED)
endif()

## Boost
if(NOT MSVC)
    find_package(Boost 1.64.0 COMPONENTS system filesystem REQUIRED)
else()
    find_package(Boost 1.64.0)
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD ON)

# Include header files directories
include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../)
# Static libraries directories
link_directories(${Boost_LIBRARY_DIRS})
# Create executable
set(EXECUTABLE_NAME TestThreadPool)
set(TEST_FILES Test.cpp)
add_executable(${EXECUTABLE_NAME} ${TEST_FILES})

## Linking up all libraries
target_link_libraries(${EXECUTABLE_NAME} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
if(NOT MSVC)
	target_link_libraries(${EXECUTABLE_NAME} pthread)
endif()

add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})

# Code coverage 
find_package(codecov)
add_coverage(${EXECUTABLE_NAME})
