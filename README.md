# Cpp-Utilities

[![Build Status][travis-badge]][travis-link]
[![Build Status][appveyor-badge]][appveyor-link]
[![codecov][codecov-badge]][codecov-link]

### Abstract

Utility classes useful in daily C++ programming.

# Thread pool

Header only library. Contains following thread pools:
- Thread pool with single task queue
- Thread pool with task queue per each thread
- Thread pool with work stealing task queue
- Boost::asio based thread pool
- [PPL](https://msdn.microsoft.com/library/dd492418.aspx) based thread pool

Inspired by Sean Parent's talk ([presentation](http://sean-parent.stlab.cc/presentations/2016-11-16-concurrency/2016-11-16-concurrency.pdf)) and this [lib](https://github.com/topcpporg/thread-pool-cpp).

[travis-badge]:      https://travis-ci.org/vukis/Utilities.svg?branch=master
[travis-link]:       https://travis-ci.org/vukis/Utilities
[appveyor-badge]:    https://ci.appveyor.com/api/projects/status/1l8srr6wo3ixnc7o?svg=true
[appveyor-link]:     https://ci.appveyor.com/project/vukis/utilities
[codecov-badge]:     https://codecov.io/gh/vukis/Utilities/branch/master/graph/badge.svg
[codecov-link]:      https://codecov.io/gh/vukis/Utilities
