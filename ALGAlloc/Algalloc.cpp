#include <cstdio>
#include <dlfcn.h>
#include "Algalloc.hpp"

#include <cstdlib>
#include <mutex>

void** g_last_alloc = nullptr;
std::mutex g_last_alloc_mutex;

bool g_sandboxed = false;

//TODO: alignment

void* wire_allocation(void* p)
{
    std::lock_guard<std::mutex> lock(g_last_alloc_mutex);
    void** ptr = static_cast<void**>(p);
    if (g_last_alloc != nullptr)
        g_last_alloc[1] = ptr;
    ptr[0] = g_last_alloc;
    ptr[1] = nullptr;
    g_last_alloc = ptr;
    return ptr + 2;
}

void* (*g_real_malloc_sym)(size_t) = nullptr;
void* real_malloc(const size_t size)
{
    if (g_real_malloc_sym == nullptr)
        g_real_malloc_sym = reinterpret_cast<void*(*)(size_t)>(dlsym(RTLD_NEXT, "malloc"));
    return g_real_malloc_sym(size);
}
void* sandbox_malloc(const size_t size)
{
    void* ptr = real_malloc(size + 2 * sizeof(void*));
    return wire_allocation(ptr);
}
void* malloc(const size_t size)
{
    if (g_sandboxed) return sandbox_malloc(size);
    return real_malloc(size);
}

void (*g_real_free_sym)(void*) = nullptr;
void real_free(void* ptr)
{
    if (g_real_free_sym == nullptr)
        g_real_free_sym = reinterpret_cast<void(*)(void*)>(dlsym(RTLD_NEXT, "free"));
    return g_real_free_sym(ptr);
}
void sandbox_free(void* p)
{
    void** ptr = static_cast<void**>(p) - 2;
    void** prev = static_cast<void**>(ptr[0]);
    void** next = static_cast<void**>(ptr[1]);
    if (prev != nullptr) prev[1] = next;
    if (next != nullptr) next[0] = prev;

    {
        std::lock_guard<std::mutex> lock(g_last_alloc_mutex);
        if (g_last_alloc == ptr)
            g_last_alloc = prev;
    }
    real_free(ptr);
}
void free(void* p)
{
    if (g_sandboxed) sandbox_free(p);
    else real_free(p);
}

void* (*g_real_calloc_sym)(size_t, size_t) = nullptr;
void* real_calloc(const size_t num, const size_t size)
{
    if (g_real_calloc_sym == nullptr)
        g_real_calloc_sym = reinterpret_cast<void*(*)(size_t, size_t)>(dlsym(RTLD_NEXT, "calloc"));
    return g_real_calloc_sym(num, size);
}
void* sandbox_calloc(const size_t num, const size_t size)
{
    void* ptr = real_calloc(num * size + 2 * sizeof(void*), 1);
    return wire_allocation(ptr);
}
void* calloc(const size_t num, const size_t size)
{
    if (g_sandboxed) return sandbox_calloc(num, size);
    return real_calloc(num, size);
}

void* (*g_real_realloc_sym)(void*, size_t) = nullptr;
void* real_realloc(void* p, const size_t new_size)
{
    if (g_real_realloc_sym == nullptr)
        g_real_realloc_sym = reinterpret_cast<void*(*)(void*, size_t)>(dlsym(RTLD_NEXT, "realloc"));
    return g_real_realloc_sym(p, new_size);
}
void* sandbox_realloc(void* p, const size_t new_size)
{
    if (p == nullptr)
        return malloc(new_size);
    if (new_size == 0)
    {
        free(p);
        return nullptr;
    }
    void** old_ptr = static_cast<void**>(p) - 2;
    void** prev = static_cast<void**>(old_ptr[0]);
    void** next = static_cast<void**>(old_ptr[1]);
    void** new_ptr = static_cast<void**>(real_realloc(old_ptr, new_size + 2 * sizeof(void*)));
    if (prev != nullptr) prev[1] = new_ptr;
    if (next != nullptr) next[0] = new_ptr;
    return new_ptr + 2;
}
void* realloc(void* p, const size_t size)
{
    if (g_sandboxed) return sandbox_realloc(p, size);
    return real_realloc(p, size);
}

void* (*g_real_aligned_alloc_sym)(size_t, size_t) = nullptr;
void* real_aligned_alloc(size_t alignment, size_t size)
{
    if (g_real_aligned_alloc_sym == nullptr)
        g_real_aligned_alloc_sym = reinterpret_cast<void*(*)(size_t, size_t)>(dlsym(RTLD_NEXT, "aligned_alloc"));
    return g_real_aligned_alloc_sym(alignment, size);
}
void* sandbox_aligned_alloc(size_t alignment, size_t size)
{
    // return __real_aligned_alloc(alignment, size);
    return malloc(size);
}
void* aligned_alloc(size_t alignment, size_t size)
{
    if (g_sandboxed) return sandbox_aligned_alloc(alignment, size);
    return real_aligned_alloc(alignment, size);
}

size_t (*g_real_malloc_usable_size_sym)(void*) = nullptr;
size_t real_malloc_usable_size(void* ptr)
{
    if (g_real_malloc_usable_size_sym == nullptr)
        g_real_malloc_usable_size_sym = reinterpret_cast<size_t(*)(void*)>(dlsym(RTLD_NEXT, "malloc_usable_size"));
    return g_real_malloc_usable_size_sym(ptr);
}
size_t sandbox_malloc_usable_size(void* ptr)
{
    if (ptr == nullptr) return 0;
    return real_malloc_usable_size(static_cast<void**>(ptr) - 2) - 2 * sizeof(void*);
}
size_t malloc_usable_size(void* ptr)
{
    if (g_sandboxed) return sandbox_malloc_usable_size(ptr);
    return real_malloc_usable_size(ptr);
}


void* (*g_real_memalign_sym)(size_t, size_t) = nullptr;
void* real_memalign(const size_t alignment, const size_t size)
{
    if (g_real_memalign_sym == nullptr)
        g_real_memalign_sym = reinterpret_cast<void*(*)(size_t, size_t)>(dlsym(RTLD_NEXT, "memalign"));
    return g_real_memalign_sym(alignment, size);
}
void* sandbox_memalign(size_t alignment, size_t size)
{
    // return __real_memalign(alignment, size);
    return malloc(size);
}
void* memalign(const size_t alignment, const size_t size)
{
    if (g_sandboxed) return sandbox_memalign(alignment, size);
    return real_memalign(alignment, size);
}

int (*g_real_posix_memalign_sym)(void**, size_t, size_t) = nullptr;
int real_posix_memalign(void** memptr, size_t alignment, size_t size)
{
    if (g_real_posix_memalign_sym == nullptr)
        g_real_posix_memalign_sym = reinterpret_cast<int(*)(void**, size_t, size_t)>(dlsym(RTLD_NEXT, "posix_memalign"));
    return g_real_posix_memalign_sym(memptr, alignment, size);
}
int sandbox_posix_memalign(void** memptr, size_t alignment, size_t size)
{
    if (size == 0)
    {
        *memptr = nullptr;
        return 0;
    }
    const int res = real_posix_memalign(memptr, alignment, size + 2 * sizeof(void*));
    if (res != 0) return res;
    *memptr = wire_allocation(*memptr);
    return res;
}
int posix_memalign(void** memptr, size_t alignment, size_t size)
{
    if (g_sandboxed) return sandbox_posix_memalign(memptr, alignment, size);
    return real_posix_memalign(memptr, alignment, size);
}

void* (*g_real_valloc_sym)(size_t) = nullptr;
void* real_valloc(const size_t size)
{
    if (g_real_valloc_sym == nullptr)
        g_real_valloc_sym = reinterpret_cast<void*(*)(size_t)>(dlsym(RTLD_NEXT, "valloc"));
    return g_real_valloc_sym(size);
}
void* sandbox_valloc(const size_t size)
{
    // return __real_valloc(size);
    return malloc(size);
}
void* valloc(size_t size)
{
    if (g_sandboxed) return sandbox_valloc(size);
    return real_valloc(size);
}

void* (*g_real_pvalloc_sym)(size_t) = nullptr;
void* real_pvalloc(const size_t size)
{
    if (g_real_pvalloc_sym == nullptr)
        g_real_pvalloc_sym = reinterpret_cast<void*(*)(size_t)>(dlsym(RTLD_NEXT, "pvalloc"));
    return g_real_pvalloc_sym(size);
}
void* sandbox_pvalloc(size_t size)
{
    // return __real_pvalloc(size);
    return malloc(size);
}
void* pvalloc(size_t size)
{
    if (g_sandboxed) return sandbox_pvalloc(size);
    return real_pvalloc(size);
}

template<>
void MemorySandbox::apply<void>(const std::function<void()>& func)
{
    applyVoid(func);
}

void MemorySandbox::cleanupFailedApply()
{
    cleanup();
}

void MemorySandbox::applyVoid(const std::function<void()>& func)
{
    if (g_sandboxed == true)
    {
        fprintf(stderr, "Another memory sandbox context already active: terminating");
        exit(-1);
    }

    prepare();
    func();
    cleanup();
}

void MemorySandbox::prepare() const
{
    std::lock_guard<std::mutex> lock(g_last_alloc_mutex);
    g_last_alloc = last_alloc_;
    g_sandboxed = true;
}

void MemorySandbox::cleanup()
{
    std::lock_guard<std::mutex> lock(g_last_alloc_mutex);
    g_sandboxed = false;
    last_alloc_ = g_last_alloc;
    g_last_alloc = nullptr;
}

void MemorySandbox::free()
{
    void** ptr = last_alloc_;
    while (ptr != nullptr)
    {
        void** prev_ptr = static_cast<void**>(ptr[0]);
        real_free(ptr);
        ptr = prev_ptr;
    }
    last_alloc_ = nullptr;
}

MemorySandbox::~MemorySandbox()
{
    free();
}