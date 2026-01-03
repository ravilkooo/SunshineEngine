#pragma once
#include <cstdint>
#include <EASTL/functional.h>
#include <EASTL/string.h>
using ResourceGUID = uint64_t;

struct ResourceHandle
{
    ResourceGUID guid;
    uint32_t version; 

    bool operator==(const ResourceHandle& other) const
    {
        return guid == other.guid && version == other.version;
    }

    bool operator!=(const ResourceHandle& other) const
    {
        return guid != other.guid || version != other.version;
    }
};

inline ResourceGUID ComputeGUID(const eastl::string& path)
{
    eastl::hash<eastl::string> hasher;
    return static_cast<ResourceGUID>(hasher(path));
}
