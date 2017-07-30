cmake_minimum_required(VERSION 3.2)

set(EXECUTABLE_NAME Test_ThreadPool)

# Test ThreadPool
set(TEST_FILES Test.cpp)

add_executable(${EXECUTABLE_NAME} ${TEST_FILES})

# Linking up all libraries
target_link_libraries(${EXECUTABLE_NAME} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
if(NOT MSVC)
	target_link_libraries(${EXECUTABLE_NAME} pthread)
endif()

setup_target_for_coverage(${EXECUTABLE_NAME} ${EXECUTABLE_NAME} coverage)
add_test(NAME ${EXECUTABLE_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME})