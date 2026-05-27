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

    //
    // Gameplay state
    //

    //CharacterState State = CharacterState::Idle;

    bool IsPlayerControlled = false;

    bool IsDead = false;
    bool IsAttacking = false;

    //
    // Input intentions
    //

    DXSM::Vector2 MoveInput = DXSM::Vector2::Zero;

    bool JumpRequested = false;
    bool AttackRequested = false;

    //
    // View rotation
    //

    float Yaw = 0.0f;
    float Pitch = 0.0f;

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
