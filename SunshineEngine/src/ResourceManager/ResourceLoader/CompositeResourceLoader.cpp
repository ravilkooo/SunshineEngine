#include <ResourceManager/IResource.h>
#include <ResourceManager/ResourceLoader/CompositeResourceLoader.h>
#include <ResourceManager/ResourceLoaderFactory.h>
#include <ResourceManager/Resources/Model.h>
#include <ResourceManager/ResourceRegistry.h>
#include <EASTL/unordered_set.h>
#include <Utils/DebugUtils.h>

IResource* CompositeResourceLoader::Load(const eastl::string& path,
    ResourceRegistry* pRegistry,
    ResourceMemoryManager* pMemMgr)
{
    // Track loading paths to detect circular dependencies
    static thread_local eastl::unordered_set<eastl::string> loadingPaths;
    
    // Check for circular dependency
    if (loadingPaths.find(path) != loadingPaths.end()) {
        printSunshineErrorMessage(("Circular dependency detected while loading: {}", path.c_str()));
        return nullptr;
    }
    
    // Mark this path as being loaded
    loadingPaths.insert(path);
    
    // 1. Resolve and load all dependencies
    eastl::vector<eastl::string> deps = ResolveDependencies(path);
    eastl::vector<ResourceGUID> loadedDependencies;

    for (const auto& depPath : deps)
    {
        // Skip empty paths
        if (depPath.empty()) {
            printSunshineErrorMessage(("Empty dependency path found while loading: {}", path.c_str()));
            continue;
        }

        ResourceGUID depGUID = ComputeGUID(depPath);
        IResource* pDepResource = pRegistry->Get(depGUID);

        if (!pDepResource)
        {
            IResourceLoader* pDepLoader = ResourceLoaderFactory::GetLoaderForFile(depPath);
            if (!pDepLoader) {
                printSunshineErrorMessage(("No loader found for dependency: {}", depPath.c_str()));
                continue;
            }

            // Recursively load the dependency
            pDepResource = pDepLoader->Load(depPath, pRegistry, pMemMgr);
            if (!pDepResource) {
                printSunshineErrorMessage(("Failed to load dependency: {}", depPath.c_str()));
                continue;
            }

            // Only register if not already registered by the loader
            if (!pRegistry->Contains(depGUID)) {
                ResourceHandle depHandle;
                depHandle.guid = depGUID;
                depHandle.version = 1;
                pRegistry->Register(depHandle, pDepResource, depPath);
            }
        }
        
        loadedDependencies.push_back(depGUID);
    }

    // 2. Create the main resource (Model in this case)
    Model* pModel = new (std::nothrow) Model();
    if (!pModel) {
        printSunshineErrorMessage(("Failed to allocate memory for model: {}", path.c_str()));
        loadingPaths.erase(path);
        return nullptr;
    }

    // 3. Link dependencies to the model
    for (const auto& depGUID : loadedDependencies)
    {
        // Get the dependency to check its type
        IResource* pDepResource = pRegistry->Get(depGUID);
        if (!pDepResource) {
            printSunshineErrorMessage(("Dependency not found in registry: {}", eastl::to_string(depGUID)));
            continue;
        }

        // Add to appropriate container based on type
        GetResourceType(pDepResource, pModel, depGUID);
    }

    // Clean up and return
    loadingPaths.erase(path);
    return pModel;
}

SunshineResource::ResourceType DetermineResourceType(const eastl::string& path)
{
    // ��������: Assets/Models/hero.mesh, Assets/Textures/tex1.dds
    auto dotPos = path.find_last_of('.');
    if (dotPos == eastl::string::npos)
        return SunshineResource::ResourceType::COUNT; // ��� ���� ����������� ���

    auto ext = path.substr(dotPos + 1);
    if (ext == "mesh") return SunshineResource::ResourceType::MESH;
    if (ext == "mat" || ext == "material") return SunshineResource::ResourceType::MATERIAL;
    if (ext == "dds" || ext == "png" || ext == "jpg") return SunshineResource::ResourceType::TEXTURE;
    if (ext == "anim") return SunshineResource::ResourceType::ANIMATION;
    // ... � �.�.

    return SunshineResource::ResourceType::COUNT;
}

eastl::vector<eastl::string> CompositeResourceLoader::ResolveDependencies(const eastl::string& path)
{
    eastl::vector<eastl::string> dependencies;

    // ������ �� ������ �������� json/xml/txt ������:
    // ����� �������, ��� ����������� ������������� � ��������� ������ �����.
    // ��������: {
    //   "meshes": ["mesh1.mesh", "mesh2.mesh"],
    //   "materials": ["mat1.mat", "mat2.mat"],
    //   "textures": ["tex1.dds", "tex2.dds"]
    // }

    // --- Pseudocode, �������� ������ ������� �� ������� ������� ---
    // std::ifstream file(path.c_str());
    // std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    //
    // JsonDocument doc = ParseJson(content); // ��� ���� xml/txt ������
    //
    // for (auto& meshPath : doc["meshes"])
    //     dependencies.push_back(meshPath.GetString());
    // for (auto& matPath : doc["materials"])
    //     dependencies.push_back(matPath.GetString());
    // for (auto& texPath : doc["textures"])
    //     dependencies.push_back(texPath.GetString());

    // --- ������ ��� �������� ���������� ������� ---
    /*
    std::ifstream file(path.c_str());
    eastl::string line;
    while (std::getline(file, line))
    {
        if (line.starts_with("dependency:"))
        {
            // dependency:Assets/Characters/hero.mesh
            eastl::string depPath = line.substr(strlen("dependency:"));
            dependencies.push_back(depPath);
        }
    }
    */

    // --- ������ ��� ������� Assimp (������) ---
    // ���� ������������ Assimp, ����� ��������� ����� � �������� �� ���� ���������, ���������� � �����:
    /*
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate);
    if (scene)
    {
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
        {
            // �������� ���� � ����, ���� �� �������
            // dependencies.push_back(meshPath);
        }
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
        {
            aiMaterial* material = scene->mMaterials[i];
            // for ������ texture type: get path
            aiString texPath;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
                dependencies.push_back(texPath.C_Str());
        }
    }
    */

    // --- ��� stub: ������ ������ ������������� ����������� ---
    // dependencies.push_back("Assets/mesh1.mesh");
    // dependencies.push_back("Assets/texture1.dds");
    // dependencies.push_back("Assets/material1.mat");

    return dependencies;
}

bool CompositeResourceLoader::CanLoad(const eastl::string& path) const
{
    return false;
}

SunshineResource::ResourceType CompositeResourceLoader::GetResourceType(IResource* pDepResource, Model* pModel, ResourceGUID depGUID) const
{
    switch (pDepResource->GetType())
    {
    case SunshineResource::ResourceType::TEXTURE:
        pModel->m_Textures.push_back(depGUID);
        break;
    case SunshineResource::ResourceType::MESH:
        pModel->m_Meshes.push_back(depGUID);
        break;
    case SunshineResource::ResourceType::MATERIAL:
        pModel->m_Materials.push_back(depGUID);
        break;
    case SunshineResource::ResourceType::SHADER:
        pModel->m_Shaders.push_back(depGUID);
        break;
    case SunshineResource::ResourceType::SKELETON:
        pModel->m_Skeleton = depGUID;
        break;
    case SunshineResource::ResourceType::ANIMATION:
        pModel->m_Animations.push_back(depGUID);
        break;
    default:
        printSunshineErrorMessage(("Unhandled resource type in dependencies: {}",
            eastl::string(SunshineResource::ResourceTypeToString(pDepResource->GetType()))));
        break;
    }
    return pDepResource->GetType();
}


