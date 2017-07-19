cmake_minimum_required(VERSION 3.2)

project(ThreadPool)

# Set binaries path
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/bin)

# Boost
find_package(Boost REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD ON)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

if(WIN32)
    add_compile_options("-D_WIN32_WINNT=0x0501") # Supress warning
    if(MSVC)
        add_compile_options("/std:c++latest") # C++17
    endif()
endif()

set(SOURCE_FILES ${PROJECT_NAME}/Test.cpp)
add_executable(Test${PROJECT_NAME} ${SOURCE_FILES})
