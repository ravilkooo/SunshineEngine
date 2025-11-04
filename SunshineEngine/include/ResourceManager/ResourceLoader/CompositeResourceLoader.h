#pragma once
#include <ResourceManager/IResource.h>
#include <EASTL/vector.h>
#include <ResourceManager/MemoryManager/ResourceMemoryManager.h>
#include <ResourceManager/ResourceLoader/IResourceLoader.h>
#include <ResourceManager/ResourceRegistry.h>

class CompositeResourceLoader : public IResourceLoader
{
public:
    /*
    IResource* Load(const eastl::string& path,
        ResourceRegistry* pRegistry,
        ResourceMemoryManager* pMemMgr) override;
    */
    IResource* Load(const eastl::string& path,
        ResourceMemoryManager* pMemMgr) override;

    eastl::vector<eastl::string> ResolveDependencies(const eastl::string& path);

    ResourceType DetermineResourceType(const eastl::string& path);
};