#pragma once

#include <EASTL/string.h>
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <ResourceManager/ResourceHandle.h>
#include <ResourceManager/ResourceRegistry.h>
#include <ResourceManager/ResourceLoaderFactory.h>
#include <ResourceManager/MemoryManager/ResourceMemoryManager.h>
#include <ResourceManager/IResource.h>

// Simple, convenient facade over the engine ResourceManager stack.
// Responsibilities:
// - Compute GUID from path
// - Reuse from registry if loaded
// - Load via appropriate IResourceLoader
// - Track refs in registry
// - Provide typed accessors
class ResourceManagerFacade {
public:
    static ResourceManagerFacade& Instance() {
        static ResourceManagerFacade inst;
        return inst;
    }

    // Load by file path (auto-detects loader by extension). Increments refcount on reuse.
    ResourceHandle LoadByPath(const eastl::string& path) {
        const ResourceGUID guid = ComputeGUID(path);

        // If already present, just bump ref and return
        if (m_registry.Contains(guid)) {
            m_registry.IncrementRefCount(guid);
            ResourceEntry* entry = m_registry.GetEntry(guid);
            return entry ? entry->handle : ResourceHandle{ guid, 0u };
        }

        // Resolve loader
        IResourceLoader* loader = ResourceLoaderFactory::GetLoaderForFile(path);
        if (!loader) {
            // No loader -> do not register; return handle with guid only
            return ResourceHandle{ guid, 0u };
        }

        // Load resource
        IResource* res = loader->Load(path, &m_memoryManager);
        if (!res) {
            return ResourceHandle{ guid, 0u };
        }

        // Post-load init
        res->PostLoadInit();

        // Register in registry
        ResourceHandle handle{ guid, 0u };
        const bool ok = m_registry.Register(handle, res, path);
        if (!ok) {
            // If registration failed, cleanup local ref and return
            res->PreUnload();
            m_memoryManager.Deallocate(res, res->GetSizeInMemory());
            return ResourceHandle{ guid, 0u };
        }

        m_registry.IncrementRefCount(guid);
        return handle;
    }

    // Raw typed getter (non-owning). Returns nullptr if not found.
    template <typename T>
    T* Get(const ResourceHandle& handle) {
        static_assert(eastl::is_base_of<IResource, T>::value, "T must derive from IResource");
        IResource* res = m_registry.Get(handle.guid);
        return res ? static_cast<T*>(res) : nullptr;
    }

    // Raw untyped getter (non-owning)
    IResource* GetRaw(const ResourceHandle& handle) {
        return m_registry.Get(handle.guid);
    }

    // Increase/decrease reference count explicitly
    void AddRef(const ResourceHandle& handle) {
        m_registry.IncrementRefCount(handle.guid);
    }

    void Release(const ResourceHandle& handle) {
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

    bool Contains(const ResourceHandle& handle) const {
        return m_registry.Contains(handle.guid);
    }

    size_t GetTotalMemoryUsage() const { return m_registry.GetTotalMemoryUsage(); }

private:
    ResourceManagerFacade() = default;
    ~ResourceManagerFacade() = default;

    ResourceManagerFacade(const ResourceManagerFacade&) = delete;
    ResourceManagerFacade& operator=(const ResourceManagerFacade&) = delete;

private:
    ResourceRegistry m_registry;
    ResourceMemoryManager m_memoryManager; // default heap-backed
};
