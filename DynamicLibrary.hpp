#ifndef ALGATORC_DYNAMICLIBRARY_HPP
#define ALGATORC_DYNAMICLIBRARY_HPP
#include <dlfcn.h>
#include <filesystem>

#include "Error.hpp"


class DynamicLibrary
{
public:
    explicit DynamicLibrary(const std::filesystem::path& path);

    template <typename T>
    T resolve(const std::string& name);

private:
    void* handle_;
};

template <typename T>
T DynamicLibrary::resolve(const std::string& name)
{
    long* ptr = static_cast<long*>(dlsym(handle_, name.c_str()));
    const char* err = dlerror();
    if (err != nullptr && !ptr)
    {
        error(ErrorType::System, ErrorPhase::Execution, "Failed to find symbol '") << name << "':" << err;
    }

    T func = reinterpret_cast<T>(ptr);
    return func;
}


#endif //ALGATORC_DYNAMICLIBRARY_HPP
