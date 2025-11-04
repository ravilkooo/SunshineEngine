#include <ResourceManager/ResourceRegistry.h>

bool ResourceRegistry::Register(const ResourceHandle& handle,
    IResource* pResource,
    const eastl::string& path)
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);

    if (m_Registry.find(handle.guid) != m_Registry.end())
    {
        return false; 
    }

    ResourceEntry entry;
    entry.handle = handle;
    entry.pResource = pResource;
    entry.path = path;
    entry.state = ResourceState::LOADED;
    entry.memorySize = pResource->GetSizeInMemory();

    m_Registry[handle.guid] = entry;
    return true;
}

IResource* ResourceRegistry::Get(ResourceGUID guid)
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);

    auto it = m_Registry.find(guid);
    if (it != m_Registry.end() && it->second.IsValid())
    {
        return it->second.pResource;
    }

    return nullptr;
}

bool ResourceRegistry::Unregister(ResourceGUID guid)
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);

    auto it = m_Registry.find(guid);
    if (it == m_Registry.end())
    {
        return false;
    }

    m_Registry.erase(it);
    return true;
}

bool ResourceRegistry::Contains(ResourceGUID guid) const
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);
    return m_Registry.find(guid) != m_Registry.end();
}


void ResourceRegistry::SetResourceState(ResourceGUID guid, ResourceState state)
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);
    auto it = m_Registry.find(guid);
    if (it != m_Registry.end())
    {
        it->second.state = state;
    }
}

ResourceState ResourceRegistry::GetResourceState(ResourceGUID guid) const
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);
    auto it = m_Registry.find(guid);
    if (it != m_Registry.end())
    {
        return it->second.state;
    }
    return ResourceState::UNLOADED;
}

void ResourceRegistry::IncrementRefCount(ResourceGUID guid)
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);
    auto it = m_Registry.find(guid);
    if (it != m_Registry.end())
    {
        ++it->second.refCount;
    }
}

void ResourceRegistry::DecrementRefCount(ResourceGUID guid)
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);
    auto it = m_Registry.find(guid);
    if (it != m_Registry.end() && it->second.refCount > 0)
    {
        --it->second.refCount;
    }
}

uint32_t ResourceRegistry::GetRefCount(ResourceGUID guid) const
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);
    auto it = m_Registry.find(guid);
    if (it != m_Registry.end())
    {
        return it->second.refCount;
    }
    return 0;
}

size_t ResourceRegistry::GetTotalMemoryUsage() const
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);
    size_t total = 0;
    for (const auto& pair : m_Registry)
    {
        total += pair.second.memorySize;
    }
    return total;
}

size_t ResourceRegistry::GetResourceCount() const
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);
    return m_Registry.size();
}

eastl::vector<ResourceEntry*> ResourceRegistry::GetAllResources()
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);
    eastl::vector<ResourceEntry*> result;
    result.reserve(m_Registry.size());
    for (auto& pair : m_Registry)
    {
        result.push_back(&pair.second);
    }
    return result;
}

ResourceEntry* ResourceRegistry::GetEntry(ResourceGUID guid)
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);
    auto it = m_Registry.find(guid);
    if (it != m_Registry.end())
    {
        return &it->second;
    }
    return nullptr;
}

const IResource* ResourceRegistry::Get(ResourceGUID guid) const
{
    std::lock_guard<std::mutex> lock(m_RegistryMutex);
    auto it = m_Registry.find(guid);
    if (it != m_Registry.end() && it->second.IsValid())
    {
        return it->second.pResource;
    }
    return nullptr;
}