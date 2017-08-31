#pragma once

#include <future>

template<typename FunctionT>
inline auto Asynchronize(FunctionT&& function)
{
    return [function](auto&&... args) {
        return [&function, &args]() {
            return std::async(std::launch::async, function, args...);
        };
    };
}

template<typename FunctionT>
inline auto UnwrapFutures(FunctionT&& function)
{
    return [&function](auto&&... futures) {
        return function(futures.get()...);
    };
}

template<typename FunctionT>
inline auto AsyncAdapter(FunctionT&& function)
{
    return [&function](auto&&... callables) {
        return [&function, &callables]() {
            return std::async(std::launch::async, UnwrapFutures(function), callables()...);
        };
    };
}
