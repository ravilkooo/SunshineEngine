#pragma once
#include <cstdint>

enum class MemoryStrategy : uint8_t
{
    HEAP_BASED = 0, 
    STACK_BASED = 1,
    POOL_BASED = 2  
};