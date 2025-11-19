#pragma once
#include <ResourceManager/MemoryManager/HeapMemoryManager.h>
#include <ResourceManager/IResource.h>
#include <ResourceManager/ResourceRegistry.h>
#include <ResourceManager/Resources/Model.h>

class IResourceLoader
{
public:
    virtual ~IResourceLoader() = default;

    virtual IResource* Load(const eastl::string& path,
        ResourceRegistry* pRegistry,
        ResourceMemoryManager* pMemMgr) = 0;

    virtual SunshineResource::ResourceType GetResourceType(IResource* pDepResource, Model* pModel, ResourceGUID depGUID) const = 0;

    virtual bool CanLoad(const eastl::string& path) const = 0;
};