#pragma once
#include <ResourceManager/ResourceHandle.h>
#include <ResourceManager/IResource.h>
#include <ResourceManager/Enums/ResourceState.h>

struct ResourceEntry
{
    ResourceHandle handle;
    IResource* pResource = nullptr;
    ResourceState state = ResourceState::UNLOADED;
    uint32_t refCount = 0;  
    size_t memorySize = 0;
    eastl::string path;

    bool IsValid() const
    {
        return pResource != nullptr && state == ResourceState::LOADED;
    }
};
