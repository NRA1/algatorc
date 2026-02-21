#include <cstdio>
#include <dlfcn.h>
#include "Algalloc.hpp"

#include <cassert>
#include <cstdlib>
#include <mutex>
#include <unistd.h>

// Holds a linked list of all non-freed allocations.
// Every node is structured like:
// 0< |    ... user data ...      |
// -1 |        next node          |
// -2 |      previous node        |
// -3 |       base address        |
// -4> | ... alignment padding ... |
// "next node" and "previous node" pointers hold address to start of "user data" of next section
// "base address" holds address of the start of allocated block (to pass to underlying free())
void** g_last_alloc = nullptr;
std::mutex g_last_alloc_mutex;

bool g_sandboxed = false;

void wire_allocation(void* user_p, void* base_p)
{
    std::lock_guard<std::mutex> lock(g_last_alloc_mutex);
    void** user_ptr = static_cast<void**>(user_p);
    if (g_last_alloc != nullptr)
        g_last_alloc[-1] = user_ptr;
    user_ptr[-1] = nullptr;
    user_ptr[-2] = g_last_alloc;
    user_ptr[-3] = base_p;
    g_last_alloc = user_ptr;
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
    void** base_ptr = static_cast<void**>(real_malloc(size + 3 * sizeof(void*)));
    void** user_ptr = base_ptr + 3;
    wire_allocation(user_ptr, base_ptr);
    return user_ptr;
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
    void** user_ptr = static_cast<void**>(p);
    void** next = static_cast<void**>(user_ptr[-1]);
    void** prev = static_cast<void**>(user_ptr[-2]);
    if (prev != nullptr) prev[-1] = next;
    if (next != nullptr) next[-2] = prev;

    {
        std::lock_guard<std::mutex> lock(g_last_alloc_mutex);
        if (g_last_alloc == user_ptr)
            g_last_alloc = prev;
    }

    void* base_ptr = user_ptr[-3];
    real_free(base_ptr);
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
    void** base_ptr = static_cast<void**>(real_calloc(num * size + 3 * sizeof(void*), 1));
    void** user_ptr = base_ptr + 3;
    wire_allocation(user_ptr, base_ptr);
    return user_ptr;
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
    void** old_user_ptr = static_cast<void**>(p);
    void** next = static_cast<void**>(old_user_ptr[-1]);
    void** prev = static_cast<void**>(old_user_ptr[-2]);
    void* old_base_ptr = old_user_ptr[-3];
    void** new_base_ptr = static_cast<void**>(real_realloc(old_base_ptr, new_size + 3 * sizeof(void*)));
    void** new_user_ptr = new_base_ptr + 3;
    if (prev != nullptr) prev[-1] = new_user_ptr;
    if (next != nullptr) next[-2] = new_user_ptr;
    return new_user_ptr;
}
void* realloc(void* p, const size_t size)
{
    if (g_sandboxed) return sandbox_realloc(p, size);
    return real_realloc(p, size);
}

void* (*g_real_memalign_sym)(size_t, size_t) = nullptr;
void* real_memalign(const size_t alignment, const size_t size)
{
    if (g_real_memalign_sym == nullptr)
        g_real_memalign_sym = reinterpret_cast<void*(*)(size_t, size_t)>(dlsym(RTLD_NEXT, "memalign"));
    return g_real_memalign_sym(alignment, size);
}
void* sandbox_memalign(const size_t alignment, const size_t size)
{
    const size_t multi = ((3 * sizeof(void*)) + alignment - 1) / alignment; // ceil of (3 * sizeof(void*)) / alignment
    void** base_ptr = static_cast<void**>(real_memalign(alignment, size + multi * alignment));
    void** user_ptr = reinterpret_cast<void**>(reinterpret_cast<char*>(base_ptr) + multi * alignment);
    wire_allocation(user_ptr, base_ptr);
    assert(reinterpret_cast<unsigned long long>(user_ptr) % alignment == 0);
    return user_ptr;
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
    const size_t multi = ((3 * sizeof(void*)) + alignment - 1) / alignment; // ceil of (3 * sizeof(void*)) / alignment
    const int res = real_posix_memalign(memptr, alignment, size + multi * alignment);
    if (res != 0) return res;

    void** base_ptr = static_cast<void**>(*memptr);
    void** user_ptr = reinterpret_cast<void**>(reinterpret_cast<char*>(base_ptr) + multi * alignment);
    wire_allocation(user_ptr, base_ptr);
    assert(reinterpret_cast<unsigned long long>(user_ptr) % alignment == 0);
    *memptr = user_ptr;
    return res;
}
int posix_memalign(void** memptr, size_t alignment, size_t size)
{
    if (g_sandboxed) return sandbox_posix_memalign(memptr, alignment, size);
    return real_posix_memalign(memptr, alignment, size);
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
    return sandbox_memalign(alignment, size);
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
    void* base_ptr = static_cast<void**>(ptr)[-3];
    return real_malloc_usable_size(base_ptr) -
        (reinterpret_cast<unsigned long long>(ptr) - reinterpret_cast<unsigned long long>(base_ptr));
}
size_t malloc_usable_size(void* ptr)
{
    if (g_sandboxed) return sandbox_malloc_usable_size(ptr);
    return real_malloc_usable_size(ptr);
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
    return sandbox_memalign(sysconf(_SC_PAGESIZE), size);
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
    const long int page_size = sysconf(_SC_PAGESIZE);
    const size_t ceiled_size = ((size + 3 * sizeof(void*)) + page_size - 1) / page_size;
    return sandbox_valloc(ceiled_size);
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
        void** prev_user_ptr = static_cast<void**>(ptr[-2]);
        void** base_ptr = static_cast<void**>(ptr[-3]);
        real_free(base_ptr);
        ptr = prev_user_ptr;
    }
    last_alloc_ = nullptr;
}

MemorySandbox::~MemorySandbox()
{
    free();
}