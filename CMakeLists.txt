cmake_minimum_required(VERSION 3.2)
project(ThreadPool)

set(EXECUTABLE_NAME Test${CMAKE_PROJECT_NAME})

# Setting default folders
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCE_FILES ${CMAKE_PROJECT_NAME}/Test.cpp)

# In CMakeModules I include code coverage for c++
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

if(NOT MSVC)
    include(${CMAKE_MODULE_PATH}/CodeCoverage.cmake)
    setup_target_for_coverage(${CMAKE_PROJECT_NAME}Coverage tests coverage)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -g -O0 -fprofile-arcs -ftest-coverage")
endif()

# C++ compile flags
# -std=c++1z -> for c++ standarts
# -Wall for WARNING
# -pedantic -> for warning between c and c++
# -pthread for portability
# -g -O0 -fprofile-arcs -ftest-coverage for code coverage

# C++17
if(MSVC)
    add_compile_options("/std:c++latest") # C++17
	add_compile_options("-D_WIN32_WINNT=0x0501") # Supress warning
else()
    add_compile_options("-std=c++1z")
endif()

# Set CPU achitecture specific compiler flags
if ("${TARGET_CPU}" STREQUAL "x86")
    set (CMAKE_SIZEOF_VOID_P 4)

    set_property (GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS FALSE)
    set_property (GLOBAL PROPERTY FIND_LIBRARY_USE_LIB32_PATHS TRUE)

    if (GCC)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
        set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -m32")
    endif ()
	
elseif ("${TARGET_CPU}" STREQUAL "x64")
    set (CMAKE_SIZEOF_VOID_P 8)

    set_property (GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)
    set_property (GLOBAL PROPERTY FIND_LIBRARY_USE_LIB32_PATHS FALSE)

    if (GCC)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
        set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -m64")
    endif ()

endif ()

# Dependencies
if(NOT MSVC)
## pthread
    find_package(Threads REQUIRED)
endif()	
## Boost
find_package(Boost 1.64.0 REQUIRED COMPONENTS system filesystem)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)

# Include header files
include_directories(${BOOST_INCLUDE_DIRS})
# Static libraries
link_directories(${BOOST_LIBRARY_DIRS})

include(CTest)
enable_testing()

# Create executable
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})
if(NOT MSVC)
	target_link_libraries(${EXECUTABLE_NAME} pthread)
endif()
target_link_libraries(${EXECUTABLE_NAME} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})