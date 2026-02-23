#ifndef ALGATORC_ALGEXIT_HPP
#define ALGATORC_ALGEXIT_HPP
#include <functional>

template<typename T>
T terminationInterceptorApply(const std::function<T()>& func, bool& termination_attempted, int& termination_code)
{
    T res;
    terminationInterceptorApply<void>([&]
    {
        res = func();
    }, termination_attempted, termination_code);
    return res;
}

template<>
void terminationInterceptorApply<void>(const std::function<void()>& func, bool& termination_attempted, int& termination_code);

#endif //ALGATORC_ALGEXIT_HPP