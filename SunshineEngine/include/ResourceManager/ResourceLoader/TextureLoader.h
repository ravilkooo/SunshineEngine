#pragma once
#include <d3d11.h>
#include <ResourceManager/ResourceLoader/IResourceLoader.h>
#include <ResourceManager/MemoryManager/ResourceMemoryManager.h>
#include <ResourceManager/ResourceRegistry.h>
#include <Graphics/GraphicsResources/Texture.h>

class TextureLoader : public IResourceLoader
{
public:
    explicit TextureLoader(ID3D11Device* device);

    IResource* Load(const eastl::string& path,
        ResourceRegistry* pRegistry,
        StackMemoryManager* pMemMgr) override;

    SunshineResource::ResourceType GetResourceType(IResource* pDepResource,
        Model* pModel,
        ResourceGUID depGUID) const override;

    bool CanLoad(const eastl::string& path) const override;
private:
    ID3D11Device* m_device;
};

