#pragma once
#include <ResourceManager/MemoryManager/HeapMemoryManager.h>
#include <ResourceManager/IResource.h>

class IResourceLoader
{
public:
    virtual ~IResourceLoader() = default;

    virtual IResource* Load(const eastl::string& path,
        ResourceMemoryManager* pMemMgr) = 0;

    virtual ResourceType GetResourceType() const = 0;

    virtual bool CanLoad(const eastl::string& path) const = 0;
};