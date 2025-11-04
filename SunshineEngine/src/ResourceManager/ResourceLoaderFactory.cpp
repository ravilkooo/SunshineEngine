#include <ResourceManager/ResourceLoaderFactory.h>
#include <cassert>

eastl::hash_map<int, eastl::unique_ptr<IResourceLoader>> ResourceLoaderFactory::m_Loaders;

void ResourceLoaderFactory::RegisterLoader(ResourceType type, eastl::unique_ptr<IResourceLoader> loader)
{
    int typeId = static_cast<int>(type);
    // ћожно добавить защиту от двойной регистрации
    assert(loader && "Loader must not be nullptr!");
    m_Loaders[typeId] = eastl::move(loader);
}

IResourceLoader* ResourceLoaderFactory::GetLoader(ResourceType type)
{
    int typeId = static_cast<int>(type);
    auto it = m_Loaders.find(typeId);
    if (it != m_Loaders.end())
        return it->second.get();
    return nullptr;
}

IResourceLoader* ResourceLoaderFactory::GetLoaderForFile(const eastl::string& path)
{
    for (auto& pair : m_Loaders)
    {
        if (pair.second->CanLoad(path))
            return pair.second.get();
    }
    return nullptr;
}
