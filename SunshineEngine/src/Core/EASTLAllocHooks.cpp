// EASTLAllocHooks.cpp
#include "Core/EASTLAllocHooks.h"
#include <EASTL/internal/config.h>
#include <new>
#include <cstdlib>

/*
void* operator new[](size_t size,
    const char* pName,
    int flags,
    unsigned debugFlags,
    const char* file,
    int line)
{
    (void)pName; (void)flags; (void)debugFlags; (void)file; (void)line;
    return ::operator new[](size);
}

void* operator new[](size_t size,
    size_t alignment,
    size_t alignmentOffset,
    const char* pName,
    int flags,
    unsigned debugFlags,
    const char* file,
    int line)
{
    (void)pName; (void)flags; (void)debugFlags; (void)file; (void)line;
    (void)alignment; (void)alignmentOffset;
    return ::operator new[](size);
}
*/

void* operator new[](size_t size, [[maybe_unused]] const char* pName, [[maybe_unused]] int flags, [[maybe_unused]] unsigned debugFlags, [[maybe_unused]] const char* file, [[maybe_unused]] int line)
{
    return ::malloc(size);
};

void* operator new[](size_t size, size_t alignment, [[maybe_unused]] size_t alignmentOffset, [[maybe_unused]] const char* pName, [[maybe_unused]] int flags, [[maybe_unused]] unsigned debugFlags, [[maybe_unused]] const char* file, [[maybe_unused]] int line)
{
    return ::_aligned_malloc(size, alignment);
    // return ::malloc(size);
};
