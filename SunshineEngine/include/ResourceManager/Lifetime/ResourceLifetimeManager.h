#pragma once
#include <ResourceManager/Enums/ResourceLifetime.h>
#include <ResourceManager/ResourceHandle.h>
#include <ResourceManager/Lifetime/ResourceLifetimeInfo.h>
#include <EASTL/vector.h>
#include <unordered_map>
#include <mutex>

class ResourceLifetimeManager
{
public:
    void RegisterResource(ResourceGUID guid, ResourceLifetime lifetime);

    void OnLevelLoad(uint32_t levelId);

    void OnLevelUnload(uint32_t levelId);

    eastl::vector<ResourceGUID> GetResourcesToLoad();
    eastl::vector<ResourceGUID> GetResourcesToUnload();

private:
    std::unordered_map<uint64_t, ResourceLifetimeInfo> m_Resources;
    std::mutex m_LifetimeMutex;
};
