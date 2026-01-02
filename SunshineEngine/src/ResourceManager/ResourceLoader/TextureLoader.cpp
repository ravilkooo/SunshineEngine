#include <ResourceManager/ResourceLoader/TextureLoader.h>
#include <Graphics/GraphicsResources/Texture.h>
#include "Utils/StringHelper.h"
#include <ResourceManager/MemoryManager/StackMemoryManager.h>

TextureLoader::TextureLoader(ID3D11Device* device) : m_device(device)
{
}

IResource* TextureLoader::Load(const AssetPath& path, ResourceRegistry* pRegistry, StackMemoryManager* pMemMgr)
{
    void* mem = pMemMgr->Allocate(sizeof(SE_G::Bind::Texture), SunshineResource::ResourceType::TEXTURE);

    if (!mem) return nullptr;

    auto tex = new (mem) SE_G::Bind::Texture(m_device, path, 0u, SE_G::Bind::PipelineStage::PIXEL_SHADER);

    return tex;
}

SunshineResource::ResourceType TextureLoader::GetResourceType(
    IResource* /*pDepResource*/,
    Model*     /*pModel*/,
    ResourceGUID /*depGUID*/) const
{
    return SunshineResource::ResourceType::TEXTURE;
}

bool TextureLoader::CanLoad(const eastl::string& path) const
{
    auto dot = path.find_last_of('.');
    if (dot == eastl::string::npos) return false;
    auto ext = path.substr(dot + 1);
    return ext == "dds" || ext == "png" || ext == "jpg";
}
