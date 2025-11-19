#pragma once
#include <cstdint>

enum class ResourceState : uint8_t
{
    UNLOADED = 0,
    LOADING = 1,
    LOADED = 2,
    FAILED = 3
};