cmake_minimum_required(VERSION 3.2)

project(ThreadPool)

set(EXECUTABLE_NAME Test${CMAKE_PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/bin)
set(SOURCE_FILES ${CMAKE_PROJECT_NAME}/Test.cpp)

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
    find_package(Threads REQUIRED)
endif()	

## Boost
find_package(Boost 1.64.0 REQUIRED COMPONENTS system filesystem)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
include_directories(${BOOST_INCLUDE_DIRS})
link_directories(${BOOST_LIBRARY_DIRS})

# Create executable
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

if(NOT MSVC)
	target_link_libraries(${EXECUTABLE_NAME} pthread)
endif()

target_link_libraries(${EXECUTABLE_NAME}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY})