#include "DynamicLibrary.hpp"

#include <dlfcn.h>

#include "../Support/Error.hpp"

DynamicLibrary::DynamicLibrary(const std::filesystem::path& path)
{
    handle_ = dlopen(path.c_str(), RTLD_NOW);
    if (!handle_)
    {
        error(ErrorType::System, ErrorPhase::Execution, "Failed to load dynamic library from path ")
        << path << ": " << dlerror();
    }

    error_func_ = resolve<char*(*)()>("__error");
    clear_error_func_ = resolve<void(*)()>("__clear_error");
}
