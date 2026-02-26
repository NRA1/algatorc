#include "DynamicLibrary.hpp"

#include <dlfcn.h>

#include "../Support/Error.hpp"

DynamicLibrary::DynamicLibrary(void* handle) : handle_(handle)
{
    error_func_ = resolve<char*(*)()>("__error");
    clear_error_func_ = resolve<void(*)()>("__clear_error");
}

DynamicLibrary::DynamicLibrary(DynamicLibrary&& other) noexcept
    : error_func_(other.error_func_), clear_error_func_(other.clear_error_func_), handle_(other.handle_),
    sandbox_(std::move(other.sandbox_))
{
    other.handle_ = nullptr;
}

void DynamicLibrary::freeAll()
{
    sandbox_.free();
}

DynamicLibrary::~DynamicLibrary()
{
    if (handle_ != nullptr)
        dlclose(handle_);
}

void DynamicLibrary::swap(DynamicLibrary& other) noexcept
{
    std::swap(handle_, other.handle_);
    std::swap(sandbox_, other.sandbox_);
    std::swap(error_func_, other.error_func_);
    std::swap(clear_error_func_, other.clear_error_func_);
}

void swap(DynamicLibrary& first, DynamicLibrary& second) noexcept
{
    first.swap(second);
}
