#ifndef ALGATORC_DYNAMICLIBRARY_HPP
#define ALGATORC_DYNAMICLIBRARY_HPP
#include <dlfcn.h>
#include <filesystem>

#include "../ALGAlloc/Algalloc.hpp"
#include "../Support/Error.hpp"


class DynamicLibrary
{
public:
    void freeAll();

protected:
    explicit DynamicLibrary(const std::filesystem::path& path);

    template <typename T>
    T resolve(const std::string& name);

    template <typename T>
    T executeInContext(const char* name, const std::function<T()>& func);

    char* (*error_func_)();
    void (*clear_error_func_)();


    virtual ~DynamicLibrary();

private:
    void* handle_;
    MemorySandbox sandbox_;
};

template <typename T>
T DynamicLibrary::resolve(const std::string& name)
{
    long* ptr = static_cast<long*>(dlsym(handle_, name.c_str()));
    const char* err = dlerror();
    if (err != nullptr && !ptr)
    {
        error(ErrorType::System, "Failed to find symbol '") << name << "':" << err;
    }

    T func = reinterpret_cast<T>(ptr);
    return func;
}

template <typename T>
T DynamicLibrary::executeInContext(const char* name, const std::function<T()>& func)
{
    return guard<T>(name, error_func_, clear_error_func_, [&]()
    {
        return sandbox_.apply<T>([&]()
        {
            return func();
        });
    });
}


#endif //ALGATORC_DYNAMICLIBRARY_HPP
