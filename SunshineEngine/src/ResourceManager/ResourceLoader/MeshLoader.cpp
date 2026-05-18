#include <Graphics/GraphicsResources/Mesh.h>

#include <ResourceManager/ResourceLoader/MeshLoader.h>
#include <ResourceManager/MemoryManager/StackMemoryManager.h>

#include <Utils/StringHelper.h>
#include <Utils/FileUtils.h>

eastl::string MeshLoader::defaultMeshes[DEFAULT_MESHES_NUM] = {
    "Box", "Plane", "Sphere", "Geosphere", "Cylinder", "Box_repeat", "ScreenAlignedQuad"
};

MeshLoader::MeshLoader(ID3D11Device* device) : m_device(device)
{
}

IResource* MeshLoader::Load(const AssetPath& path, ResourceRegistry* pRegistry, StackMemoryManager* pMemMgr)
{
    void* mem = pMemMgr->Allocate(sizeof(SE_G::Mesh), SunshineResource::ResourceType::MESH);

    if (!mem) return nullptr;

    SE_G::Mesh* mesh = nullptr;

    std::filesystem::path fp(path.GetFullPath().c_str());

    bool isDefaultMesh = false;
    for (size_t i = 0; i < DEFAULT_MESHES_NUM; i++)
    {
        isDefaultMesh |= (WStringToUtf8(path.m_assetRelativePath) == defaultMeshes[i]);
    }

    if (FileExistsNoThrow(fp) || isDefaultMesh)
    {
        mesh = new (mem) SE_G::Mesh(m_device, path);
    }
    else
    {
        // auto meshPath = AssetPath(eastl::wstring(L"Box_repeat"));
        // mesh = new (mem) SE_G::Mesh(m_device, meshPath);
        pMemMgr->Deallocate(mesh, sizeof(SE_G::Mesh));
        mesh = nullptr;
    }
    return mesh;
}

SunshineResource::ResourceType MeshLoader::GetResourceType(IResource* pDepResource, Model* pModel, ResourceGUID depGUID) const
{
    return SunshineResource::ResourceType::MESH;
}

bool MeshLoader::CanLoad(const eastl::string& path) const
{
    for (size_t i = 0; i < DEFAULT_MESHES_NUM; i++)
    {
        if (path.length() >= defaultMeshes[i].length() &&
            path.substr(path.length() - defaultMeshes[i].length()) == defaultMeshes[i]) {
            return true;
        }
    }

    auto dot = path.find_last_of('.');
    if (dot == eastl::string::npos) return false;
    auto ext = path.substr(dot + 1);
    return ext == "obj";
}
