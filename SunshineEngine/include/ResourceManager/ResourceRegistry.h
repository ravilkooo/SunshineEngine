#pragma once
#include "ResourceHandle.h"
#include "IResource.h"
#include "Structs/ResourceEntry.h"
#include <vector>
#include <mutex>
#include <EASTL/hash_map.h>
#include <EASTL/vector.h>


class ResourceRegistry
{
public:
    ResourceRegistry();
    ~ResourceRegistry() = default;

    bool Register(const ResourceHandle& handle,
        IResource* pResource,
        const eastl::string& path);

    IResource* Get(ResourceGUID guid);
    const IResource* Get(ResourceGUID guid) const;

    ResourceEntry* GetEntry(ResourceGUID guid);

    bool Unregister(ResourceGUID guid);

    bool Contains(ResourceGUID guid) const;

    void SetResourceState(ResourceGUID guid, ResourceState state);
    ResourceState GetResourceState(ResourceGUID guid) const;

    void IncrementRefCount(ResourceGUID guid);
    void DecrementRefCount(ResourceGUID guid);
    uint32_t GetRefCount(ResourceGUID guid) const;

    size_t GetTotalMemoryUsage() const;
    size_t GetResourceCount() const;
    eastl::vector<ResourceEntry*> GetAllResources();

private:
    eastl::hash_map<uint64_t, ResourceEntry> m_Registry;
    mutable std::mutex m_RegistryMutex;  
};


