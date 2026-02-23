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

MemorySandbox::~MemorySandbox()
{
    free();
}