:loop

if "%1" == "" goto :finalize
if /i "%1" == "x86" goto :x86
if /i "%1" == "x64" goto :x64
if /i "%1" == "msvc15" goto :msvc15

echo Invalid argument: '%1'
exit -1

:: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

:: Platform

:x86
set TARGET_CPU=x86
set CMAKE_GENERATOR_SUFFIX=
set BOOST_ROOT=C:\Libraries\boost_1_64_0
set BOOST_LIBRARYDIR=C:\Libraries\boost_1_64_0\lib32-msvc-14.1
shift
goto :loop

:x64
set TARGET_CPU=x64
set CMAKE_GENERATOR_SUFFIX= Win64
set BOOST_ROOT=C:\Libraries\boost_1_64_0
set BOOST_LIBRARYDIR=C:\Libraries\boost_1_64_0\lib64-msvc-14.1
shift
goto :loop

:: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

:: Toolchain

:msvc15
set TOOLCHAIN=msvc15
set CMAKE_GENERATOR=Visual Studio 15 2017
shift
goto :loop

:: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

:finalize

echo Config: %CONFIGURATION%

if "%TOOLCHAIN%" == "" goto :msvc15
if "%TARGET_CPU%" == "" goto :x86
if "%CONFIGURATION%" == "" (set CONFIGURATION=Release)

set CMAKE_CONFIGURE_FLAGS=-G "%CMAKE_GENERATOR%%CMAKE_GENERATOR_SUFFIX%" -DBOOST_INCLUDE_DIRS="%BOOST_ROOT%" -DBOOST_LIBRARY_DIRS="%BOOST_LIBRARYDIR%" -DBUILD_SHARED_LIBS=ON
set CMAKE_BUILD_FLAGS="%CONFIGURATION%"
