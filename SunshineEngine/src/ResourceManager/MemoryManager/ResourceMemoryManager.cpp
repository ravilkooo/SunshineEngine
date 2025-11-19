#include <cstdlib> 
#include <cassert>
#include <ResourceManager/MemoryManager/ResourceMemoryManager.h>
#include <ResourceManager/Enums/ResourceType.h>

void* ResourceMemoryManager::Allocate(size_t size, SunshineResource::ResourceType /*type*/)
{
    // Basic heap allocation with accounting. Replace with custom strategy if needed.
    void* ptr = std::malloc(size);
    assert(ptr && "ResourceMemoryManager::Allocate failed: nullptr returned");
    if (ptr)
        m_UsedMemory += size;
    return ptr;
}

void ResourceMemoryManager::Deallocate(void* ptr, size_t size)
{
    if (!ptr) return;
    std::free(ptr);
    // Prevent underflow in case of mismatched sizes
    size_t before = m_UsedMemory.load();
    if (before >= size)
        m_UsedMemory -= size;
    else
        m_UsedMemory = 0;
}
