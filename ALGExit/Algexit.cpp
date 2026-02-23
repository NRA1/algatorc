#include "Algexit.hpp"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>

bool g_intercept = false;
bool g_attempted = false;
int g_attempt_exit_code = 0;

void intercept_exit(const int status)
{
    g_attempted = true;
    g_attempt_exit_code = status;
    raise(SIGABRT);
}

void (*g_real_uexit)(int) = nullptr;
void real_uexit(const int status)
{
    if (g_real_uexit == nullptr)
        g_real_uexit = reinterpret_cast<void(*)(int)>(dlsym(RTLD_NEXT, "_exit"));
    return g_real_uexit(status);
}
void _exit(const int status)
{
    if (g_intercept) intercept_exit(status);
    else real_uexit(status);
    abort(); // Unreachable
}

void (*g_real_exit)(int) = nullptr;
void real_exit(const int status)
{
    if (g_real_exit == nullptr)
        g_real_exit = reinterpret_cast<void(*)(int)>(dlsym(RTLD_NEXT, "exit"));
    return g_real_exit(status);
}
void exit(const int status)
{
    if (g_intercept) intercept_exit(status);
    else real_exit(status);
    abort(); // Unreachable
}

template<>
void terminationInterceptorApply<void>(const std::function<void()>& func, bool& termination_attempted, int& termination_code)
{
    if (g_intercept)
    {
        fprintf(stderr, "Another termination interceptor already active: exiting");
        exit(-1);
    }
    g_intercept = true;
    func();
    g_intercept = false;
    if (g_attempted)
    {
        termination_attempted = true;
        termination_code = g_attempt_exit_code;
    }
    else
    {
        termination_attempted = false;
        termination_code = 0;
    }
    g_attempted = false;
    g_attempt_exit_code = 0;
}
