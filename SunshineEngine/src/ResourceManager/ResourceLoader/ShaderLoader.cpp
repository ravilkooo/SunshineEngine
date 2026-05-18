#include <Graphics/GraphicsResources/VertexShader.h>
#include <Graphics/GraphicsResources/PixelShader.h>
#include <Graphics/GraphicsResources/GeometryShader.h>

#include <ResourceManager/ResourceLoader/ShaderLoader.h>
#include <ResourceManager/MemoryManager/StackMemoryManager.h>

#include <Utils/StringHelper.h>
#include <Utils/FileUtils.h>

ShaderLoader::ShaderLoader(ID3D11Device* device) : m_device(device)
{
}

IResource* ShaderLoader::Load(const AssetPath& path, ResourceRegistry* pRegistry,
    StackMemoryManager* pMemMgr)
{
    void* mem = nullptr;

    auto shaderType = static_cast<SE_G::Bind::PipelineStage>(path.m_params.asShader.shaderType);


    switch (shaderType)
    {
    case SE_G::Bind::PipelineStage::VERTEX_SHADER:
    {
        mem = pMemMgr->Allocate(sizeof(SE_G::Bind::VertexShader), SunshineResource::ResourceType::SHADER);
        if (!mem) return nullptr;
        SE_G::Bind::VertexShader* shader = nullptr;
        std::filesystem::path fp(path.GetFullPath().c_str());

        if (FileExistsNoThrow(fp))
        {
            shader = new (mem) SE_G::Bind::VertexShader(m_device, path);
        }
        else
        {
            pMemMgr->Deallocate(shader, sizeof(SE_G::Bind::VertexShader));
            shader = nullptr;
        }
        return shader;
    }

        break;
    case SE_G::Bind::PipelineStage::PIXEL_SHADER:
    {
        mem = pMemMgr->Allocate(sizeof(SE_G::Bind::PixelShader), SunshineResource::ResourceType::SHADER);
        if (!mem) return nullptr;
        SE_G::Bind::PixelShader* shader = nullptr;
        std::filesystem::path fp(path.GetFullPath().c_str());

        if (FileExistsNoThrow(fp))
        {
            shader = new (mem) SE_G::Bind::PixelShader(m_device, path.GetFullPath());
        }
        else
        {
            pMemMgr->Deallocate(shader, sizeof(SE_G::Bind::PixelShader));
            shader = nullptr;
        }
        return shader;
    }

        break;
    case SE_G::Bind::PipelineStage::GEOMETRY_SHADER:
    {
        mem = pMemMgr->Allocate(sizeof(SE_G::Bind::GeometryShader), SunshineResource::ResourceType::SHADER);
        if (!mem) return nullptr;
        SE_G::Bind::GeometryShader* shader = nullptr;
        std::filesystem::path fp(path.GetFullPath().c_str());

        if (FileExistsNoThrow(fp))
        {
            shader = new (mem) SE_G::Bind::GeometryShader(m_device, path.GetFullPath());
        }
        else
        {
            pMemMgr->Deallocate(shader, sizeof(SE_G::Bind::GeometryShader));
            shader = nullptr;
        }
        return shader;

    }


        break;
    case SE_G::Bind::PipelineStage::COMPUTE_SHADER:
        return nullptr;
        break;
    default:
        return nullptr;
        break;
    }
}

SunshineResource::ResourceType ShaderLoader::GetResourceType(
    IResource* /*pDepResource*/,
    Model*     /*pModel*/,
    ResourceGUID /*depGUID*/) const
{
    return SunshineResource::ResourceType::SHADER;
}

bool ShaderLoader::CanLoad(const eastl::string& path) const
{
    auto dot = path.find_last_of('.');
    if (dot == eastl::string::npos) return false;
    auto ext = path.substr(dot + 1);
    return ext == "hlsl";
}
