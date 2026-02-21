#ifndef ALGATORC_ALGALLOC_HPP
#define ALGATORC_ALGALLOC_HPP
#include <functional>

class MemorySandbox
{
public:
    MemorySandbox() = default;
    MemorySandbox(MemorySandbox&) = delete;

    template<typename T>
    T apply(const std::function<T()>& func);

    void cleanupFailedApply();

    void free();

    ~MemorySandbox();
private:
    void applyVoid(const std::function<void()>& func);

    void prepare() const;
    void cleanup();

    void** last_alloc_ = nullptr;
};

template <typename T>
T MemorySandbox::apply(const std::function<T()>& func)
{
    T result;
    applyVoid([&]
    {
        result = func();
    });
    return result;
}

template<>
void MemorySandbox::apply<void>(const std::function<void()>& func);

#endif //ALGATORC_ALGALLOC_HPP
