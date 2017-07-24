cmake_minimum_required(VERSION 3.2)

project(ThreadPool)
set(EXECUTABLE_NAME Test${CMAKE_PROJECT_NAME})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/bin)
set(SOURCE_FILES ${CMAKE_PROJECT_NAME}/Test.cpp)

# Boost
find_package(Boost 1.64.0 REQUIRED COMPONENTS system filesystem)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

if(NOT MSVC)
    find_package(Threads REQUIRED)
endif()	

if(WIN32)
    add_compile_options("-D_WIN32_WINNT=0x0501") # Supress warning
endif()

if(MSVC)
    add_compile_options("/std:c++latest") # C++17
else()
    add_compile_options("-std=c++1z")
endif()

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

if(NOT MSVC)
	target_link_libraries(${EXECUTABLE_NAME} pthread)
endif()

target_link_libraries(${EXECUTABLE_NAME}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY})
	
