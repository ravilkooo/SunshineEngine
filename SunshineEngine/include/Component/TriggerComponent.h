#pragma once
#include "Component.h"
#include <EASTL/unordered_set.h>
#include <sol/sol.hpp>
#include <Utils/UUID.h>

class TriggerComponent : public Component
{
public:
    TriggerComponent();
    virtual ~TriggerComponent() = default;

    virtual const std::type_info& getType() const override
    {
        return typeid(TriggerComponent);
    }

    virtual const SE::ComponentType ComponentType() const override
    {
        return SE::ComponentType::TRIGGER;
    }

    void SetLuaCallback(sol::function callback);
    void OnEnter(SE::UUID otherUUID);
    void OnExit(SE::UUID otherUUID);

    const eastl::unordered_set<SE::UUID>& GetInsideObjects() const
    {
        return m_insideObjects;
    }

    void FromJson(const json& j) override;

private:
    sol::function m_luaCallback;
    eastl::unordered_set<SE::UUID> m_insideObjects;

public:
    SE::UUID OwnerID;
};

class TriggerComponent_Info : public Component_Info
{
public:
    TriggerComponent_Info() = default;
    virtual ~TriggerComponent_Info() = default;

    virtual const std::type_info& getType() const override
    {
        return typeid(TriggerComponent);
    }

    virtual const SE::ComponentType ComponentType() const override
    {
        return SE::ComponentType::TRIGGER;
    }

    virtual bool IsAssigned() override
    {
        return m_assignedComponent != nullptr;
    }

    json ToJson() const override;
    void FromJson(const json& j) override;
};
