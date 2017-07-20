cmake_minimum_required(VERSION 3.2)

project(ThreadPool)

# Set binaries path
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/bin)

# Boost
find_package(Boost REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD ON)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

if(WIN32)
    add_compile_options("-D_WIN32_WINNT=0x0501") # Supress warning
endif()

if(CMAKE_COMPILER_IS_MSVC)
    add_compile_options("/std:c++latest") # C++17
else()
    add_compile_options("-std=c++1z")
endif()

set(SOURCE_FILES ${CMAKE_PROJECT_NAME}/Test.cpp)
add_executable(Test${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
