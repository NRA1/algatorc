#ifndef ALGATORC_DYNAMICLIBRARY_HPP
#define ALGATORC_DYNAMICLIBRARY_HPP
#include <dlfcn.h>
#include <filesystem>
#include <stack>
#include <variant>

#include "../ALGAlloc/Algalloc.hpp"
#include "../Support/Error.hpp"


class DynamicLibrary
{
public:
    DynamicLibrary(DynamicLibrary&& other) noexcept;

    void pushMemoryLifetime();
    void popMemoryLifetime();
    void freeAll();

protected:
    template<typename T>
    static std::variant<T, std::string> tryLoadFrom(const std::filesystem::path& path);

    template <typename T>
    T resolve(const std::string& name);

    template <typename T>
    T executeInContext(const char* name, const std::function<T()>& func);

    char* (*error_func_)();
    void (*clear_error_func_)();


    explicit DynamicLibrary(void* handle);
    virtual ~DynamicLibrary();

protected:
    friend void swap(DynamicLibrary& first, DynamicLibrary& second) noexcept;
    void swap(DynamicLibrary& other) noexcept;

private:
    void* handle_;
    std::stack<MemorySandbox> sandboxes_;
};

template <typename T>
std::variant<T, std::string> DynamicLibrary::tryLoadFrom(const std::filesystem::path& path)
{
    if (!std::filesystem::exists(path))
    {
        error(ErrorType::System, "Tried to load dynamic library from path ") << path.string() << " but the path does not exist";
    }

    void* handle = dlopen(path.string().c_str(), RTLD_NOW);
    if (handle == nullptr) return std::string(dlerror());
    return T{handle};
}

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
    return guard<T>(name,
        [this]{ return sandboxes_.top().apply<char*>([this] { return error_func_(); }); },
        [this]{ sandboxes_.top().apply<void>([this] { clear_error_func_(); }); },
        [&] {
            return sandboxes_.top().apply<T>([&]()
            {
                return func();
            });
        }, [&] { sandboxes_.top().cleanupFailedApply(); });
}


#endif //ALGATORC_DYNAMICLIBRARY_HPP
