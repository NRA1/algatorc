#include "Algalloc.hpp"

template<>
void MemorySandbox::apply<void>(const std::function<void()>& func)
{
    applyVoid(func);
}

void MemorySandbox::applyVoid(const std::function<void()>& func)
{
    func();
}

MemorySandbox::MemorySandbox(MemorySandbox&& other) noexcept : last_alloc_(other.last_alloc_)
{
    other.last_alloc_ = nullptr;
}

MemorySandbox& MemorySandbox::operator=(MemorySandbox&& other) noexcept
{
    last_alloc_ = other.last_alloc_;
    other.last_alloc_ = nullptr;
    return *this;
}

void MemorySandbox::cleanupFailedApply()
{
}

void MemorySandbox::free()
{
}

void MemorySandbox::prepare() const
{
}

void MemorySandbox::cleanup()
{
}

void MemorySandbox::swap(MemorySandbox& other) noexcept
{
    std::swap(last_alloc_, other.last_alloc_);
}

MemorySandbox::~MemorySandbox()
{
    free();
}


void swap(MemorySandbox& first, MemorySandbox& second) noexcept
{
    first.swap(second);
}