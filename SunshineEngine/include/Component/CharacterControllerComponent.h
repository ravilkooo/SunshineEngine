#pragma once

#include <EASTL/vector.h>
#include <mutex>

#include <unordered_set>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include <Utils/UUID.h>

#include <Component/Component.h>
#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;


class PhysicsSystem;
class TransformComponent;

class CharacterControllerComponent : public Component
{
public:
	CharacterControllerComponent();
	~CharacterControllerComponent();
	CharacterControllerComponent(PhysicsSystem* physicsSystem,
        TransformComponent* transformComp,
        SE::UUID uuid);

    CharacterControllerComponent(const CharacterControllerComponent&) = delete;
    CharacterControllerComponent& operator=(const CharacterControllerComponent&) = delete;

    CharacterControllerComponent(CharacterControllerComponent&&) noexcept = default;
    CharacterControllerComponent& operator=(CharacterControllerComponent&&) noexcept = default;

    void Initialize(PhysicsSystem* physicsSystem, TransformComponent* transformComp,
        SE::UUID uuid);
    
    bool Initialized = false;

    SE::UUID m_uuid = SE::UUID(0u);

    //
    // Jolt character
    //

    JPH::Ref<JPH::CharacterVirtual> m_character;
    JPH::Ref<JPH::Shape> m_shape;

    //
    // Runtime state
    //

    // Local velocity
    DXSM::Vector3 m_inputVelocity = DXSM::Vector3::Zero;
    DXSM::Vector3 m_velocity = DXSM::Vector3::Zero;

    bool m_grounded = false;

    DXSM::Vector3 m_groundNormal = DXSM::Vector3(0, 1, 0);

    //
    // Movement tuning
    //

    float m_moveSpeed = 6.0f;

    float m_acceleration = 30.0f;
    float m_airAcceleration = 8.0f;

    float m_jumpSpeed = 8.0f;

    float m_gravity = -24.0f;

    float m_maxFallSpeed = -24.0f;

    bool m_enableStickToFloor = false;
    bool m_enableWalkStairs = false;


    //
    // Capsule
    //

    eastl::shared_ptr<SE::ColliderData> m_colliderData;
    // float Radius = 0.35f;
    // float Height = 1.8f;

    float m_stepHeight = 0.3f;

    float m_maxSlopeAngle = 45.0f;

    // Inherited via Component
    const std::type_info& getType() const override {
        return typeid(CharacterControllerComponent);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::CHARACTER_CONTROLLER;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    void FromJson(const json& j) override;
    void FromJson(const json& j, PhysicsSystem* physicsSystem,
        TransformComponent* transformComp, SE::UUID uuid);
private:
    void DestroyCharacter();
};

class RenderComponent_Info;
class TransformComponent_Info;

class CharacterControllerComponent_Info :
    public Component_Info
{
public:

    CharacterControllerComponent_Info() = default;
    CharacterControllerComponent_Info(
        RenderComponent_Info* rc_info,
        TransformComponent_Info* tc_info);
    ~CharacterControllerComponent_Info();

    CharacterControllerComponent_Info(const CharacterControllerComponent_Info&) = delete;
    CharacterControllerComponent_Info& operator=(const CharacterControllerComponent_Info&) = delete;

    CharacterControllerComponent_Info(CharacterControllerComponent_Info&&) noexcept = default;
    CharacterControllerComponent_Info& operator=(CharacterControllerComponent_Info&&) noexcept = default;

    // Inherited via Component
    const std::type_info& getType() const override {
        return typeid(CharacterControllerComponent_Info);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::CHARACTER_CONTROLLER;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    bool IsAssigned() override { return true; }
    eastl::unique_ptr<CharacterControllerComponent> m_assignedComponent;

    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;

    bool m_isValid = false;
    RenderComponent_Info* m_rc_info;
};

// Macro listing fields of CharacterControllerComponent to expose in Lua bindings
#ifndef CHARACTERCONTROLLER_LUA_FIELDS_APPLY
#define CHARACTERCONTROLLER_LUA_FIELDS_APPLY(F) \
    F(m_velocity) ,             \
    F(m_grounded) ,             \
    F(m_groundNormal) ,         \
    F(m_moveSpeed) ,            \
    F(m_acceleration) ,         \
    F(m_airAcceleration) ,      \
    F(m_jumpSpeed) ,            \
    F(m_gravity) ,              \
    F(m_maxFallSpeed) ,         \
    F(m_enableStickToFloor) ,   \
    F(m_enableWalkStairs) ,     \
    F(m_stepHeight) ,           \
    F(m_maxSlopeAngle)
#endif
