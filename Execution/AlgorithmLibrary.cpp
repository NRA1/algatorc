#include "AlgorithmLibrary.hpp"

#include "../Support/Guard.hpp"

std::variant<AlgorithmLibrary, std::string> AlgorithmLibrary::tryLoadFrom(const std::filesystem::path& path)
{
    return DynamicLibrary::tryLoadFrom<AlgorithmLibrary>(path);
}

AlgorithmLibrary::AlgorithmLibrary(void* handle) : DynamicLibrary(handle)
{
    execute_func_ = resolve<void*(*)(void*)>("__execute");
}

void AlgorithmLibrary::swap(AlgorithmLibrary& other) noexcept
{
    DynamicLibrary::swap(other);
    std::swap(execute_func_, other.execute_func_);
}

void* AlgorithmLibrary::execute(void* input)
{
    return executeInContext<void*>("execute", [&]
    {
        return execute_func_(input);
    });
}

void swap(AlgorithmLibrary& first, AlgorithmLibrary& second) noexcept
{
    first.swap(second);
}
