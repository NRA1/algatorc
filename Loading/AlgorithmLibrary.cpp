#include "AlgorithmLibrary.hpp"

#include "../Support/Guard.hpp"

AlgorithmLibrary::AlgorithmLibrary(const std::filesystem::path& path) : DynamicLibrary(path)
{
    execute_func_ = resolve<void*(*)(void*)>("__execute");
}

void* AlgorithmLibrary::execute(void* input) const
{
    return guard<void*>("execute", error_func_, clear_error_func_, [&]()
    {
        return execute_func_(input);
    });
}
