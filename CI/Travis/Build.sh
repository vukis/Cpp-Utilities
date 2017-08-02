#!/bin/sh

# Build & test
mkdir -p build
cd build
cmake .. -DTARGET_CPU=$TARGET_CPU -DCMAKE_BUILD_TYPE=$BUILD_CONFIGURATION -DENABLE_COVERAGE=$COVERAGE
make
ctest -C %BUILD_CONFIGURATION% --output-on-failure

# Code covarage
if [ $COVERAGE = "On" ]; then
  make gcov && make lcov
  # Creating report
  cd $TRAVIS_BUILD_DIR 
  lcov --directory . --capture --output-file coverage.info # Capture coverage info;
  lcov --remove coverage.info '/usr/*' --output-file coverage.info # Filter out system;
  lcov --list coverage.info #Debug info
fi

# Static analysis
if [ $BUILD_CONFIGURATION = "Debug" ]; then
  make cppcheck;
fi

# Miscellaneous
#- ./bin/ThreadPool-Test