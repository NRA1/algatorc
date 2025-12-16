#include "DynamicLibrary.hpp"

#include <dlfcn.h>

#include "Error.hpp"

DynamicLibrary::DynamicLibrary(const std::filesystem::path& path)
{
    handle_ = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle_)
    {
        error(ErrorType::System, ErrorPhase::Execution, "Failed to load dynamic library from path ")
        << path << ": " << dlerror();
    }

    dlerror();
}
