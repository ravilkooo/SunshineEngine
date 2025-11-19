#pragma once
#include <EASTL/vector.h>
#include <ResourceManager/Enums/ResourceLifetime.h>

struct ResourceLifetimeInfo
{
    ResourceLifetime category;
    uint32_t refCount = 0;
    eastl::vector<uint32_t> usingLevels;
};