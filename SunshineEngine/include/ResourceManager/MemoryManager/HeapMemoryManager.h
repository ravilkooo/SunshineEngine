#pragma once
#include <ResourceManager/Enums/ResourceType.h>
#include <ResourceManager/MemoryManager/ResourceMemoryManager.h>
#include <EASTL/hash_map.h>
#include <mutex>

class HeapMemoryManager : public ResourceMemoryManager
{
public:
    void* Allocate(size_t size, ResourceType type) override;
    void Deallocate(void* ptr, size_t size) override;

private:
    eastl::hash_map<void*, size_t> m_Allocations;  
    std::mutex m_AllocationMutex;
};

