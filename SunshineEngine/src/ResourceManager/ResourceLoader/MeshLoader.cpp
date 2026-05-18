#include <Graphics/GraphicsResources/Mesh.h>

#include <ResourceManager/ResourceLoader/MeshLoader.h>
#include <ResourceManager/MemoryManager/StackMemoryManager.h>

#include <Utils/StringHelper.h>
#include <Utils/FileUtils.h>

MeshLoader::MeshLoader(ID3D11Device* device) : m_device(device)
{
}

IResource* MeshLoader::Load(const AssetPath& path, ResourceRegistry* pRegistry, StackMemoryManager* pMemMgr)
{
    void* mem = pMemMgr->Allocate(sizeof(SE_G::Mesh), SunshineResource::ResourceType::MESH);

    if (!mem) return nullptr;

    SE_G::Mesh* mesh = nullptr;

    std::filesystem::path fp(path.GetFullPath().c_str());

    if (FileExistsNoThrow(fp))
    {
        mesh = new (mem) SE_G::Mesh(m_device, path);
    }
    else
    {
        auto meshPath = AssetPath(eastl::wstring(L"Box_repeat"));
        mesh = new (mem) SE_G::Mesh(m_device, meshPath);
    }
    return mesh;
}

SunshineResource::ResourceType MeshLoader::GetResourceType(IResource* pDepResource, Model* pModel, ResourceGUID depGUID) const
{
    return SunshineResource::ResourceType::MESH;
}

bool MeshLoader::CanLoad(const eastl::string& path) const
{
    if (path == "Box") return true;
    if (path == "Sphere") return true;
    if (path == "Geosphere") return true;
    if (path == "Cylinder") return true;
    if (path == "Box_repeat") return true;
    if (path == "ScreenAlignedQuad") return true;

    auto dot = path.find_last_of('.');
    if (dot == eastl::string::npos) return false;
    auto ext = path.substr(dot + 1);
    return ext == "obj";
}
