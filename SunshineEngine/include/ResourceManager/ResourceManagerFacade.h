#pragma once

#include <EASTL/string.h>
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <ResourceManager/ResourceHandle.h>
#include <ResourceManager/ResourceRegistry.h>
#include <ResourceManager/ResourceLoaderFactory.h>
#include <ResourceManager/MemoryManager/ResourceMemoryManager.h>
#include <ResourceManager/IResource.h>
#include "ResourceLoader/CompositeResourceLoader.h"
#include <Utils/DebugUtils.h>

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
    ResourceHandle LoadByPath(const eastl::string& path);

    // Raw typed getter (non-owning). Returns nullptr if not found.
    template <typename T>
    T* Get(const ResourceHandle& handle) {
        static_assert(eastl::is_base_of<IResource, T>::value, "T must derive from IResource");
        IResource* res = m_registry.Get(handle.guid);
        return res ? static_cast<T*>(res) : nullptr;
    }

    // Raw untyped getter (non-owning)
    IResource* GetRaw(const ResourceHandle& handle);


    // Increase/decrease reference count explicitly
    void AddRef(const ResourceHandle& handle);


    void Release(const ResourceHandle& handle);


    bool Contains(const ResourceHandle& handle) const;


    size_t GetTotalMemoryUsage() const;

private:
    ResourceManagerFacade() = default;
    ~ResourceManagerFacade() = default;

    ResourceManagerFacade(const ResourceManagerFacade&) = delete;
    ResourceManagerFacade& operator=(const ResourceManagerFacade&) = delete;

private:
    ResourceRegistry m_registry;
    HeapMemoryManager* m_memoryManager; // default heap-backed
};
