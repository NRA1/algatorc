#include <functional>

#include "Algexit.hpp"

template<>
void terminationInterceptorApply<void>(const std::function<void()>& func, bool& termination_attempted, int& termination_code)
{
    func();
    termination_attempted = false;
    termination_code = 0;
}