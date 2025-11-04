#include <ResourceManager/MemoryManager/StackMemoryManager.h>
#include <cassert>
#include <cstdlib>


StackMemoryManager::StackMemoryManager(size_t maxMemory)
    : ResourceMemoryManager(maxMemory)     
    , m_StackMemory(nullptr)              
    , m_StackTop(0)
{
    m_StackMemory = static_cast<uint8_t*>(malloc(maxMemory));
    assert(m_StackMemory);
}

StackMemoryManager::~StackMemoryManager() { free(m_StackMemory); }

void* StackMemoryManager::Allocate(size_t size, ResourceType /*type*/)
{
    if (m_StackTop + size > m_MaxMemory)
        return nullptr; // Out of memory
    void* ptr = m_StackMemory + m_StackTop;
    m_StackTop += size;
    m_UsedMemory += size;
    return ptr;
}

void StackMemoryManager::Deallocate(void* /*ptr*/, size_t size)
{
    m_UsedMemory -= size;
}

void StackMemoryManager::ResetToMarker(size_t marker)
{
    if (marker <= m_StackTop && marker <= m_MaxMemory)
    {
        size_t releasedMem = m_StackTop - marker;
        m_UsedMemory -= releasedMem;

        m_StackTop = marker;

        // ѕо желанию, можно обнулить пам€ть дл€ отладки
        // memset(m_StackMemory + m_StackTop, 0, releasedMem);
    }
    else
    {
        assert(false && "ResetToMarker: marker out of range");
    }
}


void StackMemoryManager::Clear()
{
    m_StackTop = 0;
    m_UsedMemory = 0;
}
