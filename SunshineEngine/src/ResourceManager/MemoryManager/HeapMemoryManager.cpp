#include <ResourceManager/MemoryManager/HeapMemoryManager.h>

void* HeapMemoryManager::Allocate(size_t size, SunshineResource::ResourceType type)
{
    void* ptr = std::malloc(size);
    if (ptr) m_UsedMemory += size;
    return ptr;
}

void HeapMemoryManager::Deallocate(void* ptr, size_t size)
{
    std::free(ptr);
    m_UsedMemory -= size;
}
