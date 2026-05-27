#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#include <Component/Component.h>
#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;

class CharacterControllerComponent : public Component
{
public:
    //
    // Jolt character
    //

    JPH::Ref<JPH::CharacterVirtual> Character;

    //
    // Runtime state
    //

    DXSM::Vector3 Velocity = DXSM::Vector3::Zero;

    bool Grounded = false;

    DXSM::Vector3 GroundNormal = DXSM::Vector3(0, 1, 0);

    //
    // Movement tuning
    //

    float MoveSpeed = 6.0f;

    float Acceleration = 30.0f;
    float AirAcceleration = 8.0f;

    float JumpSpeed = 7.5f;

    float Gravity = -24.0f;

    float MaxFallSpeed = -40.0f;

    bool EnableStickToFloor = false;
    bool EnableWalkStairs = false;


    //
    // Capsule
    //

    float Radius = 0.35f;
    float Height = 1.8f;

    float StepHeight = 0.3f;

    float MaxSlopeAngle = 45.0f;
};
