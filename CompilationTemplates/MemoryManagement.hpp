#ifndef ALGATORC_MEMORYMANAGEMENT_HPP
#define ALGATORC_MEMORYMANAGEMENT_HPP
#include <stdio.h>
#include <stddef.h>

// Compatibility with C
// ReSharper disable CppCStyleCast
// ReSharper disable CppNonInlineFunctionDefinitionInHeaderFile
// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr

#ifdef ALGATORCPP
extern "C" {
#endif

#ifdef ALGATORCPP
inline
#endif
void** __last_alloc = NULL;

#ifdef ALGATORCPP
inline
#endif
void* __wire_allocation(void* p)
{
    void** ptr = (void**)p;
    if (__last_alloc != NULL)
        __last_alloc[1] = ptr;
    ptr[0] = __last_alloc;
    ptr[1] = NULL;
    __last_alloc = ptr;
    return ptr + 2;
}

void* __real_malloc(size_t size);
void* __wrap_malloc(size_t size)
{
    void* ptr = __real_malloc(size + 2 * sizeof(void*));
    return __wire_allocation(ptr);
}

void __real_free(void* ptr);
void __wrap_free(void* p)
{
    void** ptr = (void**)p - 2;
    void** prev = (void**)ptr[0];
    void** next = (void**)ptr[1];
    if (prev != NULL) prev[1] = next;
    if (next != NULL) next[0] = prev;
    if (__last_alloc == ptr)
        __last_alloc = prev;
    __real_free(ptr);
}

void* __real_calloc(size_t num, size_t size);
void* __wrap_calloc(const size_t num, const size_t size)
{
    void* ptr = __real_calloc(num * size + 2 * sizeof(void*), 1);
    return __wire_allocation(ptr);
}

void* __real_realloc(void* p, size_t new_size);
void* __wrap_realloc(void* p, const size_t new_size)
{
    void** old_ptr = (void**)p - 2;
    void** prev = (void**)old_ptr[0];
    void** next = (void**)old_ptr[1];
    void** new_ptr = (void**)__real_realloc(old_ptr, new_size + 2 * sizeof(void*));
    if (prev != NULL) prev[1] = new_ptr;
    if (next != NULL) new_ptr[0] = new_ptr;
    return new_ptr + 2;
}
//TODO: aligned_alloc, malloc_usable_size memalign posix_memalign pvalloc valloc

void __free_all()
{
    void** ptr = __last_alloc;
    while (ptr != NULL)
    {
        void** prev_ptr = (void**)ptr[0];
        __real_free(ptr);
        ptr = prev_ptr;
    }
    __last_alloc = NULL;
}
#ifdef ALGATORCPP
}
#endif

#endif //ALGATORC_MEMORYMANAGEMENT_HPP