#include <Graphics/GraphicsResources/Texture.h>

#include <ResourceManager/ResourceLoader/TextureLoader.h>
#include <ResourceManager/MemoryManager/StackMemoryManager.h>

#include <Utils/StringHelper.h>
#include <Utils/FileUtils.h>

TextureLoader::TextureLoader(ID3D11Device* device) : m_device(device)
{
}

IResource* TextureLoader::Load(const AssetPath& path, ResourceRegistry* pRegistry,
    StackMemoryManager* pMemMgr)
{
    void* mem = pMemMgr->Allocate(sizeof(SE_G::Bind::Texture), SunshineResource::ResourceType::TEXTURE);

    if (!mem) return nullptr;

    SE_G::Bind::Texture* tex = nullptr;

    const auto p0 = path.m_assetRelativePath.find(L"Color:");
    
    std::filesystem::path fp(path.GetFullPath().c_str());

    //if (path.m_assetRelativePath.substr(0u, 6u) == eastl::wstring(L"Color:"))
    if (p0 != eastl::wstring::npos)
    {
        //SE_G::Color col = SE_G::Bind::Texture::GetColorFromPath(path.m_assetRelativePath);
        SE_G::Color col = SE_G::Bind::Texture::GetRGBAColorFromPath(path.m_assetRelativePath);
        tex = new (mem) SE_G::Bind::Texture(m_device, col, 0u, SE_G::Bind::PipelineStage::PIXEL_SHADER);
    }
    else if (FileExistsNoThrow(fp))
    {
        tex = new (mem) SE_G::Bind::Texture(m_device, path, 0u, SE_G::Bind::PipelineStage::PIXEL_SHADER);
    }
    else
    {
        // SE_G::Color col = SE_G::Colors::UnloadedTextureColor;
        // tex = new (mem) SE_G::Bind::Texture(m_device, col, 0u, SE_G::Bind::PipelineStage::PIXEL_SHADER);
        pMemMgr->Deallocate(tex, sizeof(SE_G::Bind::Texture));
        tex = nullptr;
    }
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
