#ifndef ALGATORC_GUARD_HPP
#define ALGATORC_GUARD_HPP
#include <functional>
#include <string>

#include "Error.hpp"

void guardVoid(const std::string& source, char* (*err)(), void (*clear_err)(), const std::function<void()>& func);

template<typename T>
T guard(const std::string& source, char* (*error)(), void (*clear_error)(), const std::function<T()>& func)
{
    T res;
    guardVoid(source, error, clear_error, [&]()
    {
        res = func();
    });
    return res;
}

template<>
void guard<void>(const std::string& source, char* (*error)(), void (*clear_error)(), const std::function<void()>& func);

void guardInternal(const std::function<void()>& func);

#endif //ALGATORC_GUARD_HPP