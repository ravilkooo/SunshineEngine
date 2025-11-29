#pragma once
#include <cstdint>

enum class ResourceLifetime : uint8_t
{
    GLOBAL = 0,
    LEVEL = 1, 
    TEMPORARY = 2    
};