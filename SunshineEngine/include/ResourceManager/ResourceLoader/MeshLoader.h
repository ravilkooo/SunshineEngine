#pragma once
#include <d3d11.h>
#include <ResourceManager/ResourceLoader/IResourceLoader.h>
#include <ResourceManager/MemoryManager/ResourceMemoryManager.h>
#include <ResourceManager/ResourceRegistry.h>

#define DEFAULT_MESHES_NUM 7

class MeshLoader :
    public IResourceLoader
{
public:
    explicit MeshLoader(ID3D11Device* device);

    // Inherited via IResourceLoader
    IResource* Load(const AssetPath& path, ResourceRegistry* pRegistry,
        StackMemoryManager* pMemMgr) override;

    SunshineResource::ResourceType GetResourceType(IResource* pDepResource,
        Model* pModel, ResourceGUID depGUID) const override;

    bool CanLoad(const eastl::string& path) const override;

    static eastl::string defaultMeshes[DEFAULT_MESHES_NUM];
private:
    ID3D11Device* m_device;
};

