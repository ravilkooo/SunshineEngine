#pragma once
#include <cstdint>

enum class ResourceType : uint8_t
{
    TEXTURE = 0,
    MESH = 1,
    MATERIAL = 2,
    SHADER = 3,
    SKELETON = 4,
    ANIMATION = 5,
    COUNT
};