#pragma once
#include "SunshineEngineAPI.h"
#include <typeinfo>

// SUNSHINE_ENGINE_API
class Component
{
public:
    virtual ~Component() = default;

    virtual const std::type_info& getType() const = 0;

    // ================
    // To make SUNSHINE_ENGINE_API work

    /*
    Component() = default;
    Component(Component&&) noexcept = default;
    Component& operator=(Component&&) noexcept = default;

    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    */
    // ================
};
