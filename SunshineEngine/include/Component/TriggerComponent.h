#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/EActivation.h>

#include <EASTL/unordered_set.h>

#include <Physics/CollisionUtils.h>
#include <Component/Component.h>
#include <Utils/UUID.h>

#include <sol/sol.hpp>

class TransformComponent;
class TransformComponent_Info;
class PhysicsSystem;
class RenderComponent_Info;

class TriggerComponent : public Component
{
    friend class PhysicsSystem;
public:
    TriggerComponent();
    TriggerComponent(SE::UUID objectUUID, TransformComponent* tc);
    ~TriggerComponent();
    
    TriggerComponent(const TriggerComponent&) = delete;
    TriggerComponent& operator=(const TriggerComponent&) = delete;

    TriggerComponent(TriggerComponent&&) noexcept = default;
    TriggerComponent& operator=(TriggerComponent&&) noexcept = default;

    virtual const std::type_info& getType() const override
    {
        return typeid(TriggerComponent);
    }

    static const SE::ComponentType s_componentType = SE::ComponentType::TRIGGER;
    virtual const SE::ComponentType ComponentType() const override
    {
        return s_componentType;
    }

    void SetLuaCallback(sol::function callback);
    void OnEnter(SE::UUID otherUUID);
    void OnExit(SE::UUID otherUUID);

    const eastl::unordered_set<SE::UUID>& GetInsideObjects() const
    {
        return m_insideObjects;
    }

    auto GetInsideObjects_Lua()
    {
        return eastl::ref(m_insideObjects);
    }
    
    // Setters for configuration before adding body
    void SetObjecUUID(SE::UUID objectUUID);
    void SetShape(JPH::ShapeRefC shapePtr);

    void InitTransforms();

    JPH::Body* GetBody() const;
    JPH::BodyID GetBodyID() const;

    void FromJson(const json& j) override;

private:
    sol::function m_luaCallback;
    eastl::unordered_set<SE::UUID> m_insideObjects;

private:
    PhysicsSystem* m_physicsSystem;

    TransformComponent* transformComp;
    SE::ColliderData m_colliderData;    

    SE::UUID m_objectUUID;
    JPH::Body* m_joltBody;
    JPH::BodyID m_joltBodyId;

    JPH::RVec3 m_position = JPH::RVec3::sZero();
    JPH::Quat m_orientation = JPH::Quat::sIdentity();
    JPH::ShapeRefC m_shape = nullptr;

    // Configuration properties before creating body
    static const JPH::EMotionType s_triggerMotionType = JPH::EMotionType::Kinematic;
    static const JPH::EActivation s_triggerActivation = JPH::EActivation::DontActivate;
    static const JPH::ObjectLayer s_triggerObjectLayer = SE::Layers::TRIGGER;

};

class TriggerComponent_Info : public Component_Info
{
public:
    TriggerComponent_Info() {};
    TriggerComponent_Info(RenderComponent_Info* rc_info, 
        TransformComponent_Info* tc_info);
    ~TriggerComponent_Info();

    virtual const std::type_info& getType() const override
    {
        return typeid(TriggerComponent_Info);
    }

    static const SE::ComponentType s_componentType = SE::ComponentType::TRIGGER;
    virtual const SE::ComponentType ComponentType() const override
    {
        return s_componentType;
    }

    bool IsAssigned() override { return false; }

    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;

    // Accessors for physics configuration
    SE::ColliderShapeType GetShape() const;
    void SetShape(SE::ColliderShapeType shape);

    // valid Render ColliderTech        
    bool m_isValid = false;
    RenderComponent_Info* m_rc_info;

    // All colliders settings
    eastl::shared_ptr<SE::ColliderData> m_colliderData;
};

#ifndef TRIGGER_LUA_METHODS_APPLY
#define TRIGGER_LUA_METHODS_APPLY(FM) \
    FM("setLuaCallback",        &TriggerComponent::SetLuaCallback), \
    FM("getInsideObjects",      &TriggerComponent::GetInsideObjects_Lua)
#endif
