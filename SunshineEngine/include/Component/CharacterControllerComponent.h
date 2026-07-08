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
    float m_inputVelocity = 0.0f;
    DXSM::Vector3 m_velocity = DXSM::Vector3::Zero;

    bool m_grounded = false;
    JPH::Vec3 m_groundSpeed = JPH::Vec3::sZero();

    DXSM::Vector3 m_groundNormal = DXSM::Vector3(0, 1, 0);

    //
    // Movement tuning
    //

    bool m_syncronizeYawWithCameraForwardDir = false;
    float m_turnAcceleration = 30.0f;

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

    const DXSM::Vector3& GetVelocityVec() const { return m_velocity; }
    void SetVelocityVec(const DXSM::Vector3& velocity) { m_velocity = velocity; }

    bool IsGrounded() const { return m_grounded; }
    void SetGrounded(bool grounded) { m_grounded = grounded; }

    const DXSM::Vector3& GetGroundNormal() const { return m_groundNormal; }
    void SetGroundNormal(const DXSM::Vector3& groundNormal) { m_groundNormal = groundNormal; }

    float GetTurnAcceleration() const { return m_turnAcceleration; }
    void SetTurnAcceleration(float turnAcceleration) { m_turnAcceleration = turnAcceleration; }

    float GetMoveSpeed() const { return m_moveSpeed; }
    void SetMoveSpeed(float moveSpeed) { m_moveSpeed = moveSpeed; }

    float GetAcceleration() const { return m_acceleration; }
    void SetAcceleration(float acceleration) { m_acceleration = acceleration; }

    float GetAirAcceleration() const { return m_airAcceleration; }
    void SetAirAcceleration(float airAcceleration) { m_airAcceleration = airAcceleration; }

    float GetJumpSpeed() const { return m_jumpSpeed; }
    void SetJumpSpeed(float jumpSpeed) { m_jumpSpeed = jumpSpeed; }

    float GetGravity() const { return m_gravity; }
    void SetGravity(float gravity) { m_gravity = gravity; }

    float GetMaxFallSpeed() const { return m_maxFallSpeed; }
    void SetMaxFallSpeed(float maxFallSpeed) { m_maxFallSpeed = maxFallSpeed; }

    bool GetEnableStickToFloor() const { return m_enableStickToFloor; }
    void SetEnableStickToFloor(bool enableStickToFloor) { m_enableStickToFloor = enableStickToFloor; }

    bool GetEnableWalkStairs() const { return m_enableWalkStairs; }
    void SetEnableWalkStairs(bool enableWalkStairs) { m_enableWalkStairs = enableWalkStairs; }

    float GetStepHeight() const { return m_stepHeight; }
    void SetStepHeight(float stepHeight) { m_stepHeight = stepHeight; }

    float GetMaxSlopeAngle() const { return m_maxSlopeAngle; }
    void SetMaxSlopeAngle(float maxSlopeAngle) { m_maxSlopeAngle = maxSlopeAngle; }

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

// Macro listing properties of CharacterControllerComponent to expose in Lua bindings
#ifndef CHARACTERCONTROLLER_LUA_PROPERTIES_APPLY
#define CHARACTERCONTROLLER_LUA_PROPERTIES_APPLY(FP) \
    FP(velocityVector, &CharacterControllerComponent::GetVelocityVec, &CharacterControllerComponent::SetVelocityVec) , \
    FP(grounded, &CharacterControllerComponent::IsGrounded, &CharacterControllerComponent::SetGrounded) , \
    FP(groundNormal, &CharacterControllerComponent::GetGroundNormal, &CharacterControllerComponent::SetGroundNormal) , \
    FP(moveSpeed, &CharacterControllerComponent::GetMoveSpeed, &CharacterControllerComponent::SetMoveSpeed) , \
    FP(turnAcceleration, &CharacterControllerComponent::GetTurnAcceleration, &CharacterControllerComponent::SetTurnAcceleration) , \
    FP(acceleration, &CharacterControllerComponent::GetAcceleration, &CharacterControllerComponent::SetAcceleration) , \
    FP(airAcceleration, &CharacterControllerComponent::GetAirAcceleration, &CharacterControllerComponent::SetAirAcceleration) , \
    FP(jumpSpeed, &CharacterControllerComponent::GetJumpSpeed, &CharacterControllerComponent::SetJumpSpeed) , \
    FP(gravity, &CharacterControllerComponent::GetGravity, &CharacterControllerComponent::SetGravity) , \
    FP(maxFallSpeed, &CharacterControllerComponent::GetMaxFallSpeed, &CharacterControllerComponent::SetMaxFallSpeed) , \
    FP(enableStickToFloor, &CharacterControllerComponent::GetEnableStickToFloor, &CharacterControllerComponent::SetEnableStickToFloor) , \
    FP(enableWalkStairs, &CharacterControllerComponent::GetEnableWalkStairs, &CharacterControllerComponent::SetEnableWalkStairs) , \
    FP(stepHeight, &CharacterControllerComponent::GetStepHeight, &CharacterControllerComponent::SetStepHeight) , \
    FP(maxSlopeAngle, &CharacterControllerComponent::GetMaxSlopeAngle, &CharacterControllerComponent::SetMaxSlopeAngle)
#endif
