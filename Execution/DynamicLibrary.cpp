#include "DynamicLibrary.hpp"

#include <dlfcn.h>

#include "../Support/Error.hpp"

DynamicLibrary::DynamicLibrary(void* handle) : handle_(handle)
{
    sandboxes_.emplace(MemorySandbox{});
    error_func_ = resolve<char*(*)()>("__error");
    clear_error_func_ = resolve<void(*)()>("__clear_error");
}

DynamicLibrary::DynamicLibrary(DynamicLibrary&& other) noexcept
    : error_func_(other.error_func_), clear_error_func_(other.clear_error_func_), handle_(other.handle_),
    sandboxes_(std::move(other.sandboxes_))
{
    other.handle_ = nullptr;
}

void DynamicLibrary::pushMemoryLifetime()
{
    sandboxes_.push(MemorySandbox{});
}

void DynamicLibrary::popMemoryLifetime()
{
    if (sandboxes_.size() == 1)
        throw new std::runtime_error("Cannot pop default memory lifetime.");
    sandboxes_.top().free();
    sandboxes_.pop();
}

void DynamicLibrary::freeAll()
{
    while (sandboxes_.size() != 1)
    {
        sandboxes_.top().free();
        sandboxes_.pop();
    }
    sandboxes_.top().free();
}

DynamicLibrary::~DynamicLibrary()
{
    if (handle_ != nullptr)
        dlclose(handle_);
}

void DynamicLibrary::swap(DynamicLibrary& other) noexcept
{
    std::swap(handle_, other.handle_);
    std::swap(sandboxes_, other.sandboxes_);
    std::swap(error_func_, other.error_func_);
    std::swap(clear_error_func_, other.clear_error_func_);
}

void swap(DynamicLibrary& first, DynamicLibrary& second) noexcept
{
    first.swap(second);
}
