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

    bool m_isPlayerControlled = false;

    bool m_isDead = false;
    bool m_isAttacking = false;

    //
    // Input intentions
    //

    DXSM::Vector2 m_moveInput = DXSM::Vector2::Zero;

    bool m_jumpRequested = false;
    bool m_attackRequested = false;

    //
    // View rotation
    //

    float m_yaw = 0.0f;
    float m_pitch = 0.0f;

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

// Macro listing fields of CharacterComponent to expose in Lua bindings
#ifndef CHARACTERCOMPONENT_LUA_FIELDS_APPLY
#define CHARACTERCOMPONENT_LUA_FIELDS_APPLY(F) \
    F(m_isPlayerControlled) ,   \
    F(m_isDead) ,               \
    F(m_isAttacking) ,          \
    F(m_moveInput) ,            \
    F(m_jumpRequested) ,        \
    F(m_attackRequested) ,      \
    F(m_yaw) ,                  \
    F(m_pitch)
#endif
