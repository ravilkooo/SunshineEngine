#pragma once
#include <Component/Component.h>

#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;

/*
enum class CharacterState
{
    Idle,
    Move,
    Jump,
    Fall,
    Attack,
    Dead
};
*/

class CharacterComponent : public Component
{
public:
    CharacterComponent() = default;
    ~CharacterComponent() = default;

    CharacterComponent(const CharacterComponent&) = delete;
    CharacterComponent& operator=(const CharacterComponent&) = delete;

    CharacterComponent(CharacterComponent&&) noexcept = default;
    CharacterComponent& operator=(CharacterComponent&&) noexcept = default;

    //
    // Gameplay state
    //

    //CharacterState State = CharacterState::Idle;

    bool m_isPlayerControlled = false;

    bool m_isDead = false;
    bool m_isAttacking = false;

    //
    // Input intentions
    //

    DXSM::Vector2 m_moveInput = DXSM::Vector2::Zero;
    DXSM::Vector2 m_yawPitchDeltaInput = DXSM::Vector2::Zero;

    bool m_jumpRequested = false;
    bool m_attackRequested = false;

    // Grab objects

    bool m_grabRequested = false;
    bool m_releaseRequested = false;
    bool m_throwRequested = false;

    // Interactions

    bool m_bounced = false;
    float m_bounceSpeed = 0.0f;
    DXSM::Vector3 m_bounceNormal = DXSM::Vector3(0, 1, 0);

    //
    // View rotation
    //

    float m_yaw = 0.0f;
    float m_pitch = 0.0f;


    bool GetIsPlayerControlled() const { return m_isPlayerControlled; };
    void SetIsPlayerControlled(bool isPlayerControlled) { m_isPlayerControlled = isPlayerControlled; };

    bool GetIsDead() const { return m_isDead; };
    void SetIsDead(bool isDead) { m_isDead = isDead; };

    bool GetIsAttacking() const { return m_isAttacking; };
    void SetIsAttacking(bool isAttacking) { m_isAttacking = isAttacking; };

    const DXSM::Vector2& GetMoveInput() const { return m_moveInput; };
    void SetMoveInput(const DXSM::Vector2& moveInput) { m_moveInput = moveInput; };
    
    const DXSM::Vector2& GetYawPitchInput() const { return m_yawPitchDeltaInput; };
    void SetYawPitchInput(const DXSM::Vector2& yawPitchInput) { m_yawPitchDeltaInput = yawPitchInput; };

    bool GetJumpRequested() const { return m_jumpRequested; };
    void SetJumpRequested(bool jumpRequested) { m_jumpRequested = jumpRequested; };

    bool GetGrabRequested() const { return m_grabRequested; };
    void SetGrabRequested(bool grabRequested) { m_grabRequested = grabRequested; };

    bool GetAttackRequested() const { return m_attackRequested; };
    void SetAttackRequested(bool attackRequested) { m_attackRequested = attackRequested; };

    float GetYaw() const { return m_yaw; };
    void SetYaw(float yaw) { m_yaw = yaw; };

    float GetPitch() const { return m_pitch; };
    void SetPitch(float pitch) { m_pitch = pitch; };

    // Inherited via Component
    const std::type_info& getType() const override {
        return typeid(CharacterComponent);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::CHARACTER;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    void FromJson(const json& j) override;
};

class CharacterComponent_Info :
    public Component_Info
{
public:
    CharacterComponent_Info();
    ~CharacterComponent_Info() = default;

    CharacterComponent_Info(const CharacterComponent_Info&) = delete;
    CharacterComponent_Info& operator=(const CharacterComponent_Info&) = delete;

    CharacterComponent_Info(CharacterComponent_Info&&) noexcept = default;
    CharacterComponent_Info& operator=(CharacterComponent_Info&&) noexcept = default;

    // Inherited via Component
    const std::type_info& getType() const override {
        return typeid(CharacterComponent_Info);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::CHARACTER;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    bool IsAssigned() override { return true; }
    eastl::unique_ptr<CharacterComponent> m_assignedComponent;

    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;
};

#ifndef CHARACTERCOMPONENT_LUA_PROPERTIES_APPLY
#define CHARACTERCOMPONENT_LUA_PROPERTIES_APPLY(FP) \
    FP(isPlayerControlled, &CharacterComponent::GetIsPlayerControlled, &CharacterComponent::SetIsPlayerControlled), \
    FP(isDead, &CharacterComponent::GetIsDead, &CharacterComponent::SetIsDead), \
    FP(isAttacking, &CharacterComponent::GetIsAttacking, &CharacterComponent::SetIsAttacking), \
    FP(moveInput, &CharacterComponent::GetMoveInput, &CharacterComponent::SetMoveInput), \
    FP(yawPitchInput, &CharacterComponent::GetYawPitchInput, &CharacterComponent::SetYawPitchInput), \
    FP(jumpRequested, &CharacterComponent::GetJumpRequested, &CharacterComponent::SetJumpRequested), \
    FP(grabRequested, &CharacterComponent::GetGrabRequested, &CharacterComponent::SetGrabRequested), \
    FP(attackRequested, &CharacterComponent::GetAttackRequested, &CharacterComponent::SetAttackRequested), \
    FP(yaw, &CharacterComponent::GetYaw, &CharacterComponent::SetYaw), \
    FP(pitch, &CharacterComponent::GetPitch, &CharacterComponent::SetPitch)
#endif
