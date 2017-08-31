# Cpp-Utilities

[![Build Status][travis-badge]][travis-link]
[![Build Status][appveyor-badge]][appveyor-link]
[![codecov][codecov-badge]][codecov-link]

### Abstract

Header only utility classes useful in daily C++ programming.

# Thread pool

Contains following thread pools:
- [Thread pool](https://github.com/vukis/Cpp-Utilities/blob/master/ThreadPool/SingleQueueThreadPool.h) based on single task queue
- [Thread pool](https://github.com/vukis/Cpp-Utilities/blob/master/ThreadPool/MultiQueueThreadPool.h) based on task queue per each thread
- [Thread pool](https://github.com/vukis/Cpp-Utilities/blob/master/ThreadPool/WorkStealingThreadPool.h) based on work stealing task queue
- [Thread pool](https://github.com/vukis/Cpp-Utilities/blob/master/ThreadPool/AsioThreadPool.h) based on boost::asio
- [Thread pool](https://github.com/vukis/Cpp-Utilities/blob/master/ThreadPool/PplThreadPool.h) based on [PPL](https://msdn.microsoft.com/library/dd492418.aspx)

[Test code](https://github.com/vukis/Cpp-Utilities/blob/master/ThreadPool/Test.cpp).

Inspired by Sean Parent's talk ([presentation](http://sean-parent.stlab.cc/presentations/2016-11-16-concurrency/2016-11-16-concurrency.pdf)) and this [lib](https://github.com/topcpporg/thread-pool-cpp).

# Asynchronize

[Adapters](https://github.com/vukis/Cpp-Utilities/blob/master/Common/Asynchronize.h) that allow to create acyclic graph of parallel computing. [Test code](https://github.com/vukis/Cpp-Utilities/blob/master/Common/Test.cpp).

Inspired by concurrency examples from [this](http://www.bfilipek.com/2017/08/cpp17stl-review.html) book. Better alternative is to implement own `std::future` like [this](https://github.com/FelixPetriconi/future_proposal/blob/master/proposal.md) one. 

# Iterator adapters

TODO


[travis-badge]:      https://travis-ci.org/vukis/Utilities.svg?branch=master
[travis-link]:       https://travis-ci.org/vukis/Utilities
[appveyor-badge]:    https://ci.appveyor.com/api/projects/status/1l8srr6wo3ixnc7o?svg=true
[appveyor-link]:     https://ci.appveyor.com/project/vukis/utilities
[codecov-badge]:     https://codecov.io/gh/vukis/Utilities/branch/master/graph/badge.svg
[codecov-link]:      https://codecov.io/gh/vukis/Utilities
