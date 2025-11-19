#pragma once
#include "Enums/ResourceType.h"
#include "ResourceHandle.h"

class IResource
{
public:
    virtual ~IResource() = default;

    virtual SunshineResource::ResourceType GetType() const = 0;

    virtual ResourceGUID GetGUID() const = 0;

    virtual size_t GetSizeInMemory() const = 0;

    virtual bool PostLoadInit() { return true; }

    virtual bool PreUnload() { return true; }

    virtual void AddReference() { ++m_RefCount; }
    virtual void RemoveReference() { if (m_RefCount > 0) --m_RefCount; }
    virtual uint32_t GetReferenceCount() const { return m_RefCount; }

protected:
    uint32_t m_RefCount = 0;
};