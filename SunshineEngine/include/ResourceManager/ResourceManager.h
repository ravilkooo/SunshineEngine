#pragma once
#include <d3d11.h>
#include "Enums/MemoryStrategy.h"
#include <EASTL/string.h>
#include "Enums/ResourceType.h"
#include "ResourceHandle.h"
#include "ResourceRegistry.h"
#include "Lifetime/ResourceLifetimeManager.h"
#include <EASTL/unique_ptr.h>
#include <EASTL/queue.h>
#include "MemoryManager/ResourceMemoryManager.h"

class ResourceManager
{
public:
    ResourceManager(ID3D11Device* pDevice,
        ID3D11DeviceContext* pContext);
    ~ResourceManager();

    bool Initialize(size_t maxMemory = 512 * 1024 * 1024,
        MemoryStrategy strategy = MemoryStrategy::HEAP_BASED);

    // ============ General API ============

    template<typename T>
    T* LoadResource(const eastl::string& path)
    {
        ResourceGUID guid = ComputeGUID(path);
        return LoadResourceByGUID<T>(guid, path);
    }

    template<typename T>
    T* GetResource(const eastl::string& path)
    {
        ResourceGUID guid = ComputeGUID(path);
        IResource* pResource = m_Registry.Get(guid);
        return dynamic_cast<T*>(pResource);
    }

    bool UnloadResource(const eastl::string& path);

    // ============ Level Manager ============

    void OnLevelLoad(uint32_t levelId);
    void OnLevelUnload(uint32_t levelId);

    // Update load/unload resources
    void Update();

    // ============ СОСТАВНЫЕ РЕСУРСЫ ============

    // Загрузить группу ресурсов одновременно
    bool LoadResourceGroup(const eastl::string& groupPath);
    bool UnloadResourceGroup(const eastl::string& groupPath);

    // ============ СТАТИСТИКА ============

    void PrintMemoryStatistics() const;
    size_t GetTotalMemoryUsage() const;
    size_t GetResourceCount() const;

private:
    template<typename T>
    T* LoadResourceByGUID(ResourceGUID guid, const eastl::string& path)
    {
        if (m_Registry.Contains(guid))
        {
            return dynamic_cast<T*>(m_Registry.Get(guid));
        }

        ResourceType type = DetermineResourceType(path);

        IResourceLoader* pLoader = ResourceLoaderFactory::GetLoader(type);
        if (!pLoader)
        {
            return nullptr;
        }

        IResource* pResource = pLoader->Load(path, m_pMemoryManager.get());
        if (!pResource)
        {
            return nullptr;
        }

        ResourceHandle handle;
        handle.guid = guid;
        handle.version = 1;

        if (!m_Registry.Register(handle, pResource, path))
        {
            delete pResource;
            return nullptr;
        }

        if (!pResource->PostLoadInit())
        {
            m_Registry.Unregister(guid);
            delete pResource;
            return nullptr;
        }

        m_Registry.SetResourceState(guid, ResourceState::LOADED);

        return dynamic_cast<T*>(pResource);
    }

    ResourceType DetermineResourceType(const eastl::string& path) const;

    eastl::unique_ptr<ResourceMemoryManager> m_pMemoryManager;
    ResourceRegistry m_Registry;
    ResourceLifetimeManager m_LifetimeManager;

    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pContext;

    eastl::queue<ResourceGUID> m_LoadQueue;
    eastl::queue<ResourceGUID> m_UnloadQueue;
    std::mutex m_QueueMutex;
};
