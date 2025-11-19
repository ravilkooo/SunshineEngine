#include <ResourceManager/ResourceManagerFacade.h>

ResourceHandle ResourceManagerFacade::LoadByPath(const eastl::string& path)
{
    const ResourceGUID guid = ComputeGUID(path);

    // If already present, just bump ref and return
    if (m_registry.Contains(guid)) {
        m_registry.IncrementRefCount(guid);
        ResourceEntry* entry = m_registry.GetEntry(guid);
        return entry ? entry->handle : ResourceHandle{ guid, 0u };
    }

    // Create or get the composite loader
    static CompositeResourceLoader compositeLoader;
    IResource* res = compositeLoader.Load(path, &m_registry, &m_memoryManager);

    if (!res) {
        printSunshineErrorMessage(("Failed to load resource: {}", path.c_str()));
        return ResourceHandle{ guid, 0u };
    }

    // Post-load initialization
    res->PostLoadInit();

    // Register in registry if not already registered by the loader
    if (!m_registry.Contains(guid)) {
        ResourceHandle handle{ guid, 1u };
        if (!m_registry.Register(handle, res, path)) {
            printSunshineErrorMessage(("Failed to register resource: {}", path.c_str()));
            res->PreUnload();
            m_memoryManager.Deallocate(res, res->GetSizeInMemory());
            return ResourceHandle{ guid, 0u };
        }
    }
    else
    {
        // If already registered, just increment ref count
        m_registry.IncrementRefCount(guid);
    }

    return ResourceHandle{ guid, 1u };
}

IResource* ResourceManagerFacade::GetRaw(const ResourceHandle& handle)
{
    return m_registry.Get(handle.guid);
}

void ResourceManagerFacade::AddRef(const ResourceHandle& handle)
{
    m_registry.IncrementRefCount(handle.guid);
}

void ResourceManagerFacade::Release(const ResourceHandle& handle)
{
    // Decrement ref and unload if hits zero
    const uint32_t before = m_registry.GetRefCount(handle.guid);
    if (before == 0) return;
    m_registry.DecrementRefCount(handle.guid);
    const uint32_t after = m_registry.GetRefCount(handle.guid);
    if (after == 0) {
        IResource* res = m_registry.Get(handle.guid);
        if (res) {
            res->PreUnload();
            m_memoryManager.Deallocate(res, res->GetSizeInMemory());
        }
        m_registry.Unregister(handle.guid);
    }
}

bool ResourceManagerFacade::Contains(const ResourceHandle& handle) const
{
    return m_registry.Contains(handle.guid);
}

