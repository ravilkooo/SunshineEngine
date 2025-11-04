#include <ResourceManager/IResource.h>
#include <ResourceManager/ResourceLoader/CompositeResourceLoader.h>
#include <ResourceManager/ResourceLoaderFactory.h>
#include <ResourceManager/Resources/Model.h>
#include <ResourceManager/ResourceRegistry.h>

IResource* CompositeResourceLoader::Load(const eastl::string& path,
    // ResourceRegistry* pRegistry,
    ResourceMemoryManager* pMemMgr)
{
    eastl::vector<eastl::string> deps = ResolveDependencies(path);

    for (const auto& depPath : deps)
    {
        ResourceGUID depGUID = ComputeGUID(depPath);
        // IResource* pDepResource = pRegistry->Get(depGUID);

        /*
        if (!pDepResource)
        {
            IResourceLoader* pDepLoader =
                ResourceLoaderFactory::GetLoaderForFile(depPath);

            if (!pDepLoader)
                return nullptr;

            pDepResource = pDepLoader->Load(depPath, pMemMgr);
            if (!pDepResource)
                return nullptr;

            // Регистрируем зависимость
            ResourceHandle depHandle;
            depHandle.guid = depGUID;
            depHandle.version = 1;
            pRegistry->Register(depHandle, pDepResource, depPath);
        }
        */
    }

    // 3. Теперь загружаем сам ресурс, зная что все зависимости готовы
    Model* pModel = new Model();

    // Заполняем GUIDs зависимостей
    for (const auto& depPath : deps)
    {
        ResourceGUID depGUID = ComputeGUID(depPath);
        ResourceType depType = DetermineResourceType(depPath);

        switch (depType)
        {
        case ResourceType::MESH:
            pModel->m_Meshes.push_back(depGUID);
            break;
        case ResourceType::MATERIAL:
            pModel->m_Materials.push_back(depGUID);
            break;
        case ResourceType::TEXTURE:
            pModel->m_Textures.push_back(depGUID);
            break;
            // ...
        }
    }

    return pModel;
}

ResourceType DetermineResourceType(const eastl::string& path)
{
    // Например: Assets/Models/hero.mesh, Assets/Textures/tex1.dds
    auto dotPos = path.find_last_of('.');
    if (dotPos == eastl::string::npos)
        return ResourceType::COUNT; // Или твой специальный тип

    auto ext = path.substr(dotPos + 1);
    if (ext == "mesh") return ResourceType::MESH;
    if (ext == "mat" || ext == "material") return ResourceType::MATERIAL;
    if (ext == "dds" || ext == "png" || ext == "jpg") return ResourceType::TEXTURE;
    if (ext == "anim") return ResourceType::ANIMATION;
    // ... и т.д.

    return ResourceType::COUNT;
}

eastl::vector<eastl::string> CompositeResourceLoader::ResolveDependencies(const eastl::string& path)
{
    eastl::vector<eastl::string> dependencies;

    // Пример на случаи ассетных json/xml/txt файлов:
    // Будем считать, что зависимости перечисляются в отдельной секции файла.
    // Например: {
    //   "meshes": ["mesh1.mesh", "mesh2.mesh"],
    //   "materials": ["mat1.mat", "mat2.mat"],
    //   "textures": ["tex1.dds", "tex2.dds"]
    // }

    // --- Pseudocode, реальный парсер зависит от формата ресурса ---
    // std::ifstream file(path.c_str());
    // std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    //
    // JsonDocument doc = ParseJson(content); // или свой xml/txt парсер
    //
    // for (auto& meshPath : doc["meshes"])
    //     dependencies.push_back(meshPath.GetString());
    // for (auto& matPath : doc["materials"])
    //     dependencies.push_back(matPath.GetString());
    // for (auto& texPath : doc["textures"])
    //     dependencies.push_back(texPath.GetString());

    // --- Пример для простого текстового формата ---
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

    // --- Пример для формата Assimp (модель) ---
    // Если используется Assimp, можно загрузить сцену и пройтись по всем текстурам, материалам и мешам:
    /*
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate);
    if (scene)
    {
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
        {
            // Получить путь к мешу, если он внешний
            // dependencies.push_back(meshPath);
        }
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
        {
            aiMaterial* material = scene->mMaterials[i];
            // for каждый texture type: get path
            aiString texPath;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
                dependencies.push_back(texPath.C_Str());
        }
    }
    */

    // --- Для stub: просто вернём фиксированные зависимости ---
    // dependencies.push_back("Assets/mesh1.mesh");
    // dependencies.push_back("Assets/texture1.dds");
    // dependencies.push_back("Assets/material1.mat");

    return dependencies;
}


