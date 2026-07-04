#pragma once
#include "SunshineEngineAPI.h"
#include <typeinfo>
#include <EASTL/shared_ptr.h>
#include <Component/ComponentType.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

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
    virtual const SE::ComponentType ComponentType() const = 0;
    
    // JSON serialization hooks. Default implementations do nothing.
    virtual json ToJson() const { return json::object(); }
    virtual void FromJson(const json& /*j*/) { }
};

class Component_Info
{
public:
    Component_Info() = default;
    virtual ~Component_Info() = default;

    Component_Info(const Component_Info&) = delete;
    Component_Info& operator=(const Component_Info&) = delete;

    Component_Info(Component_Info&&) noexcept = default;
    Component_Info& operator=(Component_Info&&) noexcept = default;

    virtual const std::type_info& getType() const = 0;
    virtual const SE::ComponentType ComponentType() const = 0;

    virtual bool IsAssigned() = 0;

    // JSON serialization hooks. Default implementations do nothing.
    virtual json ToJson() const { return json::object(); }
    virtual void FromJson(const json& /*j*/) { }

    // eastl::shared_ptr<Component> m_assignedComponent;
};