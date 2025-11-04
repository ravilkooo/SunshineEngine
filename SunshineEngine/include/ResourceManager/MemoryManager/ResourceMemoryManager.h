#pragma once

#include <EASTL/atomic.h>
#include <cstddef>

enum class ResourceType : uint8_t;

class ResourceMemoryManager
{
public:
    ResourceMemoryManager(size_t maxMemory = 512 * 1024 * 1024)
        : m_MaxMemory(maxMemory), m_UsedMemory{ 0 }
    {
    }

    virtual ~ResourceMemoryManager() = default;

    virtual void* Allocate(size_t size, ResourceType type);
    virtual void Deallocate(void* ptr, size_t size);

    size_t GetUsedMemory() const { return m_UsedMemory; }
    size_t GetMaxMemory() const { return m_MaxMemory; }
    float GetMemoryUsagePercent() const
    {
        return (float)m_UsedMemory / (float)m_MaxMemory * 100.0f;
    }

    virtual void Clear() {}

protected:
    size_t m_MaxMemory;
    eastl::atomic<size_t> m_UsedMemory;
};
