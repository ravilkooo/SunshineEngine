#pragma once
#include <cstdint>

namespace SunshineResource 
{
    enum class ResourceType : uint8_t
    {
        TEXTURE = 0,
        MESH = 1,
        SHADER = 2,
        COUNT
    };
    // Helper function to convert ResourceType to string
    const char* ResourceTypeToString(ResourceType type);
}
