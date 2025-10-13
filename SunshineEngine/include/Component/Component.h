#pragma once
#include "SunshineEngineAPI.h"
#include <typeinfo>

// SUNSHINE_ENGINE_API
class Component
{
public:
    Component() = default;
    virtual ~Component() = default;

    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;

    Component(Component&&) noexcept = default;
    Component& operator=(Component&&) noexcept = default;

    virtual const std::type_info& getType() const = 0;
};
