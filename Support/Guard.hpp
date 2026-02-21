#ifndef ALGATORC_GUARD_HPP
#define ALGATORC_GUARD_HPP
#include <functional>
#include <string>

#include "Error.hpp"

void guardVoid(const std::string& source, const std::function<char*()>& err, const std::function<void()>& clear_err,
    const std::function<void()>& func, const std::function<void()>& on_failure = []{});

template<typename T>
T guard(const std::string& source, const std::function<char*()>& err, const std::function<void()>& clear_err,
    const std::function<T()>& func, const std::function<void()>& on_failure = []{})
{
    T res;
    guardVoid(source, err, clear_err, [&]()
    {
        res = func();
    }, on_failure);
    return res;
}

template<>
void guard<void>(const std::string& source, const std::function<char*()>& err, const std::function<void()>& clear_err,
    const std::function<void()>& func, const std::function<void()>& on_failure);

void guardInternal(const std::function<void()>& func);

#endif //ALGATORC_GUARD_HPP