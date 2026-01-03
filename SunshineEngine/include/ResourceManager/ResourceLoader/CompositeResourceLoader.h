#pragma once
#include <ResourceManager/IResource.h>
#include <EASTL/vector.h>
#include <ResourceManager/MemoryManager/ResourceMemoryManager.h>
#include <ResourceManager/ResourceLoader/IResourceLoader.h>
#include <ResourceManager/ResourceRegistry.h>

class CompositeResourceLoader : public IResourceLoader
{
public:
    
    IResource* Load(const AssetPath& path,
        ResourceRegistry* pRegistry,
        StackMemoryManager* pMemMgr) override;

    eastl::vector<AssetPath> ResolveDependencies(const AssetPath& path);

    SunshineResource::ResourceType GetResourceType(IResource* pDepResource, Model* pModel, ResourceGUID depGUID) const override;

    bool CanLoad(const eastl::string& path) const override;

    SunshineResource::ResourceType DetermineResourceType(const eastl::string& path);
};