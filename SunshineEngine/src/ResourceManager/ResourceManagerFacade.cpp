#include <ResourceManager/ResourceManagerFacade.h>

void ResourceManagerFacade::Initialize(size_t maxMemorySize)
{
    auto& rm = ResourceManagerFacade::Instance();
    rm.m_memoryManager = eastl::make_unique<StackMemoryManager>(maxMemorySize);
}

ResourceHandle ResourceManagerFacade::LoadByPath(const AssetPath& path)
{
    auto fullPath = WStringToUtf8(path.GetFullPath());
    const ResourceGUID guid = ComputeGUID(fullPath);

    // If already present, just bump ref and return
    if (m_registry.Contains(guid)) {
        m_registry.IncrementRefCount(guid);
        ResourceEntry* entry = m_registry.GetEntry(guid);
        return entry ? entry->handle : ResourceHandle{ guid, 0u };
    }

    IResourceLoader* loader = ResourceLoaderFactory::GetLoaderForFile(fullPath);

    if (!loader) {
        static CompositeResourceLoader compositeLoader;
        loader = &compositeLoader;
    }

    IResource* res = loader->Load(path, &m_registry, m_memoryManager.get());

    if (!res) {
        printSunshineErrorMessage(("Failed to load resource: {}", fullPath.c_str()));
        return ResourceHandle{ guid, 0u };
    }

    // Post-load initialization
    res->PostLoadInit();

    // Register in registry if not already registered by the loader
    if (!m_registry.Contains(guid)) {
        ResourceHandle handle{ guid, 1u };
        if (!m_registry.Register(handle, res, fullPath)) {
            printSunshineErrorMessage(("Failed to register resource: {}", fullPath.c_str()));
            res->PreUnload();
            m_memoryManager->Deallocate(res, res->GetSizeInMemory());
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
            m_memoryManager->Deallocate(res, res->GetSizeInMemory());
        }
        m_registry.Unregister(handle.guid);
    }
}

bool ResourceManagerFacade::Contains(const ResourceHandle& handle) const
{
    return m_registry.Contains(handle.guid);
}

