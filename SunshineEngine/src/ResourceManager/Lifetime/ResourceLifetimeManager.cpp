#include <ResourceManager/Enums/ResourceLifetime.h>
#include <ResourceManager/Lifetime/ResourceLifetimeInfo.h>

void ResourceLifetimeManager::RegisterResource(ResourceGUID guid, ResourceLifetime lifetime)
{
    std::lock_guard<std::mutex> lock(m_LifetimeMutex);
    ResourceLifetimeInfo info;
    info.category = lifetime;
    info.refCount = 0;
    m_Resources[guid] = info;
}
#include <ResourceManager/Lifetime/ResourceLifetimeManager.h>

void ResourceLifetimeManager::OnLevelLoad(uint32_t levelId)
{
    for (auto& pair : m_Resources)
    {
        auto& info = pair.second;

        if (eastl::find(info.usingLevels.begin(), info.usingLevels.end(), levelId)
            == info.usingLevels.end())
        {
            continue;
        }

        info.refCount++;
    }
}

void ResourceLifetimeManager::OnLevelUnload(uint32_t levelId)
{
    for (auto& pair : m_Resources)
    {
        auto& info = pair.second;

        if (eastl::find(info.usingLevels.begin(), info.usingLevels.end(), levelId)
            == info.usingLevels.end())
        {
            continue;
        }

        if (info.refCount > 0)
        {
            info.refCount--;
        }
    }
}

eastl::vector<ResourceGUID> ResourceLifetimeManager::GetResourcesToLoad()
{
    eastl::vector<ResourceGUID> result;

    for (auto& pair : m_Resources)
    {
        const auto& info = pair.second;

        if (info.refCount > 0)
        {
            result.push_back(pair.first);
        }
    }

    return result;
}

eastl::vector<ResourceGUID> ResourceLifetimeManager::GetResourcesToUnload()
{
    eastl::vector<ResourceGUID> result;

    for (auto& pair : m_Resources)
    {
        const auto& info = pair.second;

        if (info.refCount == 0)
        {
            result.push_back(pair.first);
        }
    }

    return result;
}

