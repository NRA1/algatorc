#include "AlgorithmLibrary.hpp"

#include "../Support/Guard.hpp"

AlgorithmLibrary::AlgorithmLibrary(const std::filesystem::path& path) : DynamicLibrary(path)
{
    execute_func_ = resolve<void*(*)(void*)>("__execute");
}

void* AlgorithmLibrary::execute(void* input)
{
    return executeInContext<void*>("execute", [&]
    {
        return execute_func_(input);
    });
}