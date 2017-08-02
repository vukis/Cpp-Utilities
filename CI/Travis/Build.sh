#!/bin/sh

# Build & test
echo Run build...
mkdir -p build
cd build
cmake .. -DTARGET_CPU=$TARGET_CPU -DCMAKE_BUILD_TYPE=$BUILD_CONFIGURATION -DENABLE_COVERAGE=$COVERAGE
make
echo Run tests...
ctest -C $BUILD_CONFIGURATION --output-on-failure

# Static analysis
if [ $BUILD_CONFIGURATION == "debug" ]; then
  echo Run cppcheck...
  make cppcheck
fi

# Code covarage
if [ $COVERAGE == "On" ]; then
  echo Run code coverage...
  make gcov && make lcov
  # Creating report
  cd $TRAVIS_BUILD_DIR 
  lcov --directory . --capture --output-file coverage.info # Capture coverage info
  lcov --remove coverage.info '/usr/*' --output-file coverage.info # Filter out system
  lcov --list coverage.info #Debug info
  echo Uploading report to CodeCov.io...
  bash <(curl -s https://codecov.io/bash) || echo "Codecov did not collect coverage reports"
fi

# Miscellaneous
#- ./bin/ThreadPool-Test