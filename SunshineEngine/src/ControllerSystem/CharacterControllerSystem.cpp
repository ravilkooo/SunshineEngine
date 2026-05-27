#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#include <Physics/PhysicsSystem.h>

#include "ControllerSystem/CharacterControllerSystem.h"

#include "Component/CharacterComponent.h"
#include "Component/CharacterControllerComponent.h"

#include "Scene.h"

#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;

CharacterControllerSystem::CharacterControllerSystem(Scene* scene, PhysicsSystem* physicsSystem)
    : m_scene(scene), m_PhysicsSystem(physicsSystem)
{
}

void CharacterControllerSystem::SetScene(Scene* scene)
{
    m_scene = scene;
}

void CharacterControllerSystem::SetPhysicsSystem(PhysicsSystem* physicsSystem)
{
    m_PhysicsSystem = physicsSystem;
}

void CharacterControllerSystem::Update(float deltaTime)
{
    for (auto& pair : m_scene->uuidToObjectMap)
    {
        UpdateCharacter(pair.second.get(), deltaTime);
    }
}

void CharacterControllerSystem::UpdateCharacter(
    GameObject* gameObj,
    float deltaTime)
{
    auto charComp = gameObj->GetComponent<CharacterComponent>();
    auto charContrComp = gameObj->GetComponent<CharacterControllerComponent>();

    if (!charComp || !charContrComp)
    {
        return;
	}

    ApplyMovementInput(charComp, charContrComp, deltaTime);

    ApplyGravity(charComp, charContrComp, deltaTime);

    ApplyJump(charComp, charContrComp);

    UpdatePhysics(charComp, charContrComp, deltaTime);

    UpdateGroundState(charComp, charContrComp);

    SynchronizeTransforms(gameObj);

    ClearFrameState(charComp, charContrComp);
}

void CharacterControllerSystem::ApplyMovementInput(
    eastl::shared_ptr<CharacterComponent> character,
    eastl::shared_ptr<CharacterControllerComponent> controller,
    float deltaTime)
{
    DXSM::Vector2 input = character->MoveInput;

    if (input.Length() > 1.0f)
    {
        input.Normalize();
    }

    DXSM::Vector3 desiredVelocity(
        input.x * controller->MoveSpeed,
        controller->Velocity.y,
        input.y * controller->MoveSpeed
    );

    float accel =
        controller->Grounded
        ? controller->Acceleration
        : controller->AirAcceleration;

    controller->Velocity.x = std::lerp(
        controller->Velocity.x,
        desiredVelocity.x,
        accel * deltaTime);

    controller->Velocity.z = std::lerp(
        controller->Velocity.z,
        desiredVelocity.z,
        accel * deltaTime
    );
}

void CharacterControllerSystem::ApplyGravity(
    eastl::shared_ptr<CharacterComponent> character,
    eastl::shared_ptr<CharacterControllerComponent> controller,
    float deltaTime)
{
    if (controller->Grounded)
    {
        return;
    }

    controller->Velocity.y +=
        controller->Gravity * deltaTime;

    if (controller->Velocity.y <
        controller->MaxFallSpeed)
    {
        controller->Velocity.y =
            controller->MaxFallSpeed;
    }
}

void CharacterControllerSystem::ApplyJump(
    eastl::shared_ptr<CharacterComponent> character,
    eastl::shared_ptr<CharacterControllerComponent> controller)
{
    if (!character->JumpRequested)
    {
        return;
    }

    if (!controller->Grounded)
    {
        return;
    }

    controller->Velocity.y =
        controller->JumpSpeed;

    controller->Grounded = false;
}

void CharacterControllerSystem::UpdatePhysics(
    eastl::shared_ptr<CharacterComponent> character,
    eastl::shared_ptr<CharacterControllerComponent> controller,
    float deltaTime)
{
    if (!controller->Character)
    {
        return;
    }

    controller->Character->SetLinearVelocity(
        JPH::Vec3(
            controller->Velocity.x,
            controller->Velocity.y,
            controller->Velocity.z
        )
    );

    JPH::CharacterVirtual::ExtendedUpdateSettings update_settings;
    DXSM::Vector3 upVector = controller->GroundNormal;
    JPH::Vec3 joltUpVector(upVector.x, upVector.y, upVector.z);
    JPH::Vec3 joltDownVector(-upVector.x, -upVector.y, -upVector.z);
    if (!controller->EnableStickToFloor)
    {
        update_settings.mStickToFloorStepDown = JPH::Vec3::sZero();
    }
    else
    {
        update_settings.mStickToFloorStepDown = joltDownVector * update_settings.mStickToFloorStepDown.Length();
    }
    if (!controller->EnableWalkStairs)
    {
        update_settings.mWalkStairsStepUp = JPH::Vec3::sZero();
    }
    else
    {
        update_settings.mWalkStairsStepUp = joltUpVector * update_settings.mWalkStairsStepUp.Length();
    }

    JPH::PhysicsSystem& physSystem = m_PhysicsSystem->GetWorld();

    controller->Character->ExtendedUpdate(
        deltaTime,
        JPH::Vec3(0.0f, controller->Gravity, 0.0f), update_settings,
        physSystem.GetDefaultBroadPhaseLayerFilter(SE::Layers::MOVING), // SE::Layers
        physSystem.GetDefaultLayerFilter(SE::Layers::MOVING), // SE::Layers
        {},
        {},
        m_PhysicsSystem->GetAllocator()
    );
}

void CharacterControllerSystem::UpdateGroundState(
    eastl::shared_ptr<CharacterComponent> character,
    eastl::shared_ptr<CharacterControllerComponent> controller)
{
    controller->Grounded =
        controller->Character->GetGroundState() ==
        JPH::CharacterBase::EGroundState::OnGround;
}

void CharacterControllerSystem::ClearFrameState(
    eastl::shared_ptr<CharacterComponent> character,
    eastl::shared_ptr<CharacterControllerComponent> controller)
{
    character->JumpRequested = false;
}

void CharacterControllerSystem::SynchronizeTransforms(GameObject* gameObj)
{
    auto charComp = gameObj->GetComponent<CharacterComponent>();
    auto charContrComp = gameObj->GetComponent<CharacterControllerComponent>();
	auto transformComp = gameObj->GetComponent<TransformComponent>();

    if (!charComp || !charContrComp || !transformComp)
    {
        return;
    }

    JPH::RVec3 charPos = charContrComp->Character->GetPosition();
    transformComp->m_position = DXSM::Vector3(charPos.GetX(), charPos.GetY(), charPos.GetZ());

    DXSM::Quaternion _quat = transformComp->GetAbsoluteWorldRotation_quat();
    const JPH::Quat targetRot(_quat.x, _quat.y, _quat.z, _quat.w);

    charContrComp->Character->SetRotation(
        JPH::Quat::sRotation(
            JPH::Vec3::sAxisY(),
            charComp->Yaw
        )
    );
}


/*
void CharacterControllerSystem::Update(float dt)
{
    for (Entity e : m_entities)
    {
        auto& character = e.Get<CharacterComponent>();
        auto& controller = e.Get<CharacterControllerComponent>();


        //
        // Movement input
        //

        DXSM::Vector3 desired =
        {
            character->MoveInput.x,
            0.0f,
            character->MoveInput.y
        };

        if (glm::length(desired) > 1.0f)
        {
            desired = glm::normalize(desired);
        }

        controller->Velocity.x = desired.x * controller->MoveSpeed;
        controller->Velocity.z = desired.z * controller->MoveSpeed;


        //
        // Gravity
        //

        if (!controller->Grounded)
        {
            controller->Velocity.y += controller->Gravity * dt;

            if (controller->Velocity.y < controller->MaxFallSpeed)
            {
                controller->Velocity.y = controller->MaxFallSpeed;
            }
        }


        //
        // Jump
        //

        if (character->JumpRequested && controller->Grounded)
        {
            controller->Velocity.y = controller->JumpSpeed;
        }

        character->JumpRequested = false;


        //
        // Jolt movement
        //

        controller->Character->SetLinearVelocity(
            JPH::Vec3(
                controller->Velocity.x,
                controller->Velocity.y,
                controller->Velocity.z
            )
        );

        controller->Character->ExtendedUpdate(
            dt,
            JPH::Vec3(0, controller->Gravity, 0),
            {},
            {},
            {},
            {}
        );


        //
        // Ground state
        //

        controller->Grounded =
            controller->Character->GetGroundState() ==
            JPH::CharacterBase::EGroundState::OnGround;
    }
}
*/