#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#include <Physics/PhysicsSystem.h>

#include "ControllerSystem/CharacterControllerSystem.h"

#include "Component/TransformComponent.h"
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
    DXSM::Vector2 input = character->m_moveInput;

    if (input.Length() > 1.0f)
    {
        input.Normalize();
    }

    DXSM::Vector3 desiredVelocity(
        input.x * controller->m_moveSpeed,
        controller->m_velocity.y,
        input.y * controller->m_moveSpeed
    );

    float accel =
        controller->m_grounded
        ? controller->m_acceleration
        : controller->m_airAcceleration;

    controller->m_velocity.x = std::lerp(
        controller->m_velocity.x,
        desiredVelocity.x,
        accel * deltaTime);

    controller->m_velocity.z = std::lerp(
        controller->m_velocity.z,
        desiredVelocity.z,
        accel * deltaTime
    );
}

void CharacterControllerSystem::ApplyGravity(
    eastl::shared_ptr<CharacterComponent> character,
    eastl::shared_ptr<CharacterControllerComponent> controller,
    float deltaTime)
{
    if (controller->m_grounded)
    {
        return;
    }

    controller->m_velocity.y +=
        controller->m_gravity * deltaTime;

    if (controller->m_velocity.y <
        controller->m_maxFallSpeed)
    {
        controller->m_velocity.y =
            controller->m_maxFallSpeed;
    }
}

void CharacterControllerSystem::ApplyJump(
    eastl::shared_ptr<CharacterComponent> character,
    eastl::shared_ptr<CharacterControllerComponent> controller)
{
    if (!character->m_jumpRequested)
    {
        return;
    }

    if (!controller->m_grounded)
    {
        return;
    }

    controller->m_velocity.y =
        controller->m_jumpSpeed;

    controller->m_grounded = false;
}

void CharacterControllerSystem::UpdatePhysics(
    eastl::shared_ptr<CharacterComponent> character,
    eastl::shared_ptr<CharacterControllerComponent> controller,
    float deltaTime)
{
    if (!controller->m_character)
    {
        return;
    }

    controller->m_character->SetLinearVelocity(
        JPH::Vec3(
            controller->m_velocity.x * cos(character->m_yaw) + controller->m_velocity.z * sin(character->m_yaw),
            controller->m_velocity.y,
            controller->m_velocity.z * cos(character->m_yaw) - controller->m_velocity.x * sin(character->m_yaw)
        )
    );

    JPH::CharacterVirtual::ExtendedUpdateSettings update_settings;
    DXSM::Vector3 upVector = controller->m_groundNormal;
    JPH::Vec3 joltUpVector(upVector.x, upVector.y, upVector.z);
    JPH::Vec3 joltDownVector(-upVector.x, -upVector.y, -upVector.z);
    if (!controller->m_enableStickToFloor)
    {
        update_settings.mStickToFloorStepDown = JPH::Vec3::sZero();
    }
    else
    {
        update_settings.mStickToFloorStepDown = joltDownVector * update_settings.mStickToFloorStepDown.Length();
    }
    if (!controller->m_enableWalkStairs)
    {
        update_settings.mWalkStairsStepUp = JPH::Vec3::sZero();
    }
    else
    {
        update_settings.mWalkStairsStepUp = joltUpVector * update_settings.mWalkStairsStepUp.Length();
    }

    JPH::PhysicsSystem& physSystem = m_PhysicsSystem->GetWorld();

    controller->m_character->ExtendedUpdate(
        deltaTime,
        JPH::Vec3(0.0f, controller->m_gravity, 0.0f), update_settings,
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
    controller->m_grounded =
        controller->m_character->GetGroundState() ==
        JPH::CharacterBase::EGroundState::OnGround;
}

void CharacterControllerSystem::ClearFrameState(
    eastl::shared_ptr<CharacterComponent> character,
    eastl::shared_ptr<CharacterControllerComponent> controller)
{
    character->m_jumpRequested = false;
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

    JPH::RVec3 charPos = charContrComp->m_character->GetPosition();
    transformComp->m_position = DXSM::Vector3(charPos.GetX(), charPos.GetY(), charPos.GetZ());

    charContrComp->m_character->SetRotation(
        JPH::Quat::sRotation(
            JPH::Vec3::sAxisY(),
            charComp->m_yaw
        )
    );


    JPH::Quat quatRot = charContrComp->m_character->GetRotation();
    transformComp->m_rotation =
        DXSM::Vector3(DXSM::Quaternion(quatRot.mValue.mF32).ToEuler()
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
            character->m_moveInput.x,
            0.0f,
            character->m_moveInput.y
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

        if (character->m_jumpRequested && controller->Grounded)
        {
            controller->Velocity.y = controller->JumpSpeed;
        }

        character->m_jumpRequested = false;


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