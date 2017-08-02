cmake_minimum_required(VERSION 3.2)
project(Cpp-Utilities)

# Setting default folders
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# In CMakeModules included:
#   - code coverage 
#   - static analyzer
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
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

# Enable code coverage
find_package(codecov)

include(CTest)
enable_testing()

# Recurse into subdirectories with cmake projects
add_subdirectory(ThreadPool)
# add_subdirectory(Common)

# Add make target for cppcheck
file(GLOB_RECURSE 
     SOURCE_FILES_TO_ANALYZE 
	 "Common/*.cpp" "Common/*.h"
     "ThreadPool/*.cpp" "ThreadPool/*.h")
include(${CMAKE_MODULE_PATH}/CppCheck.cmake)

coverage_evaluate()