cmake_minimum_required(VERSION 3.2)
project(Cpp-Utilities)

# set(EXECUTABLE_NAME Test_${CMAKE_PROJECT_NAME})

# Setting default folders
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# In CMakeModules I include code coverage for c++
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
# Load and run *.cmake files
#include(${CMAKE_MODULE_PATH}/CodeCoverage.cmake)
#setup_target_for_coverage(${PROJECT_NAME}-Coverage ThreadPool-Test ThreadPool-Coverage)

# C++ compiler flags
if(MSVC)
    # /std:c++latest -> for c++ standarts
	# "-D_WIN32_WINNT=0x0501 -> supress warning in boost
    add_compile_options("/std:c++latest")
	add_compile_options("-D_WIN32_WINNT=0x0501")
else()
    # -std=c++1z -> for c++ standarts
    # -Wall for WARNING
    # -pedantic -> for warning between c and c++
    # -pthread for portability
    # -g -O0 -fprofile-arcs -ftest-coverage for code coverage
    add_compile_options("-std=c++1z")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -g -O0 -fprofile-arcs -ftest-coverage")
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
#if(NOT MSVC)
#    find_package(Threads REQUIRED)
#endif()	
## Boost
#find_package(Boost 1.64.0 REQUIRED COMPONENTS)
#system filesystem
#set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_MULTITHREAD ON)

# Include header files directories
#include_directories(${BOOST_INCLUDE_DIRS} ThreadPool)
# Static libraries directories
#link_directories(${BOOST_LIBRARY_DIRS})

# enable code coverage
find_package(codecov)

include(CTest)
enable_testing()

# recurse into subdirectories
add_subdirectory(ThreadPool)

# evaluate coverage
coverage_evaluate()


# Test ThreadPool
#set(TEST_FILES ${CMAKE_PROJECT_NAME}/Test.cpp)

#add_executable(${EXECUTABLE_NAME} ${TEST_FILES})

# Linking up all libraries
#target_link_libraries(${EXECUTABLE_NAME} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
#if(NOT MSVC)
#	target_link_libraries(${EXECUTABLE_NAME} pthread)
#endif()

#setup_target_for_coverage(${EXECUTABLE_NAME} ${EXECUTABLE_NAME} coverage)
#add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})