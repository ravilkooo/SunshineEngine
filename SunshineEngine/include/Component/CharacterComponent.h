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
};