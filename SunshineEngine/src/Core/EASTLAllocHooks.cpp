// EASTLAllocHooks.cpp
#include "Core/EASTLAllocHooks.h"
#include <EABase/eabase.h>
#include <EASTL/internal/config.h>
#include <EASTL/unordered_map.h>
#include <new>
#include <cstdlib>



///////////////////////////////////////////////////////////////////////////////
// operator new used by EASTL
///////////////////////////////////////////////////////////////////////////////

void* operator new(size_t size)
{
    return _aligned_offset_malloc(size, 16, 0);
}

void* operator new[](size_t size)
{
    return _aligned_offset_malloc(size, 16, 0);
}

void* operator new[](size_t size, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
    return _aligned_offset_malloc(size, 16, 0);
}

void* operator new[](size_t size, size_t alignment, size_t /*alignmentOffset*/, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
    return _aligned_offset_malloc(size, alignment, 0);
}

void* operator new(size_t size, size_t alignment)
{
    return _aligned_offset_malloc(size, alignment, 0);
}

void* operator new(size_t size, size_t alignment, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
{
    return _aligned_offset_malloc(size, alignment, 0);
}

void* operator new[](size_t size, size_t alignment)
{
    return _aligned_offset_malloc(size, alignment, 0);
}

void* operator new[](size_t size, size_t alignment, const std::nothrow_t&)EA_THROW_SPEC_NEW_NONE()
{
    return _aligned_offset_malloc(size, alignment, 0);
}

void operator delete(void* p, std::size_t sz) EA_THROW_SPEC_DELETE_NONE()
{
    _aligned_free(p); EA_UNUSED(sz);
}

void operator delete[](void* p, std::size_t sz) EA_THROW_SPEC_DELETE_NONE()
{
    _aligned_free(p); EA_UNUSED(sz);
}

void operator delete(void* p) EA_THROW_SPEC_DELETE_NONE()
{
    _aligned_free(p);
}

void operator delete[](void* p) noexcept
{
    _aligned_free(p);
}


/*
void* operator new[](size_t size, [[maybe_unused]] const char* pName, [[maybe_unused]] int flags, [[maybe_unused]] unsigned debugFlags, [[maybe_unused]] const char* file, [[maybe_unused]] int line)
{
    return ::malloc(size);
};

void* operator new[](size_t size, size_t alignment, [[maybe_unused]] size_t alignmentOffset, [[maybe_unused]] const char* pName, [[maybe_unused]] int flags, [[maybe_unused]] unsigned debugFlags, [[maybe_unused]] const char* file, [[maybe_unused]] int line)
{
    return ::_aligned_malloc(size, alignment);
    // return ::malloc(size);
};

void operator delete[](void* p) noexcept
{
    //_aligned_free(p);
    free(p);
}

void operator delete(void* p, std::size_t sz) EA_THROW_SPEC_DELETE_NONE()
{
    _aligned_free(p); EA_UNUSED(sz);
}

void operator delete[](void* p, std::size_t sz) EA_THROW_SPEC_DELETE_NONE()
{
    _aligned_free(p); EA_UNUSED(sz);
}

void operator delete(void* p) EA_THROW_SPEC_DELETE_NONE()
{
    _aligned_free(p);
}
*/