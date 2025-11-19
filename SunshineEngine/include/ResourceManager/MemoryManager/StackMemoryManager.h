#pragma once
#include <cstdint>
#include <ResourceManager/MemoryManager/ResourceMemoryManager.h>
#include <ResourceManager/Enums/ResourceType.h>

class StackMemoryManager : public ResourceMemoryManager
{
public:
    StackMemoryManager(size_t maxMemory);
    ~StackMemoryManager();

    void* Allocate(size_t size, SunshineResource::ResourceType type) override;
    void Deallocate(void* ptr, size_t size) override;

    size_t CreateMarker() const { return m_StackTop; }
    void ResetToMarker(size_t marker);
    void Clear() override;

private:
    uint8_t* m_StackMemory;
    size_t m_StackTop = 0;
};

