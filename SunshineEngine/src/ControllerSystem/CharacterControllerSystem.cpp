#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#include <Physics/PhysicsSystem.h>

#include "ControllerSystem/CharacterControllerSystem.h"

#include "Component/TransformComponent.h"
#include "Component/PhysicsComponent.h"
#include "Component/TriggerComponent.h"
#include "Component/MovingPlatformComponent.h"
#include "Component/CharacterComponent.h"
#include "Component/CharacterControllerComponent.h"

#include <Scene.h>
#include <CameraManager.h>
#include <Graphics/Utils/Camera.h>

#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;


bool SECharacterContactListener::OnContactValidate(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2)
{
    return true;
}

void SECharacterContactListener::OnContactAdded(const JPH::CharacterVirtual* inCharacter,
    const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2,
    JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings)
{
    auto layer = m_bodyInterface->GetObjectLayer(inBodyID2);

    if (layer == SE::Layers::MOVING)
    {
        // Handle pushing moving objects
        return;
    }
    if (layer == SE::Layers::TRIGGER)
    {
        // Add check isSensor?
        // if (not sensor) return;

        TriggerCharacterExitEvent ev;
        ev.Trigger = SE::UUID((uint64_t)m_bodyInterface->GetUserData(inBodyID2));
        ev.Character = SE::UUID((uint64_t)inCharacter->GetUserData());

        {
            std::lock_guard<std::mutex> lock(m_enterMutex);
            m_enterQueue.push_back(ev);
        }

        TriggerCharacterOverlapKey key;
        key.TriggerBody = inBodyID2;
        key.Character = inCharacter->GetID();

        {
            std::lock_guard<std::mutex> lock(m_exitMutex);
            m_activeOverlaps[key] = ev;
        }
        return;
    }
}

void SECharacterContactListener::OnContactRemoved(const JPH::CharacterVirtual* inCharacter,
    const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2)
{
    const JPH::CharacterID a = inCharacter->GetID();

    std::lock_guard<std::mutex> lock(m_exitMutex);

    TriggerCharacterOverlapKey key1{ inBodyID2, a };

    auto it = m_activeOverlaps.find(key1);

    if (it == m_activeOverlaps.end())
        return;

    // кидаем exit-событие
    m_exitQueue.push_back(it->second);

    // удаляем активное пересечение
    m_activeOverlaps.erase(it);
}

void SECharacterContactListener::SetBodyInterface(PhysicsSystem* physicsSystem)
{
    m_physicsSystem = physicsSystem;
    m_bodyInterface = &physicsSystem->Bodies();
}

void SECharacterContactListener::FetchExitEvents(eastl::vector<TriggerCharacterExitEvent>& outEvents)
{
    std::lock_guard<std::mutex> lock(m_exitMutex);

    outEvents.insert(outEvents.end(), m_exitQueue.begin(), m_exitQueue.end());
    m_exitQueue.clear();
}

void SECharacterContactListener::FetchEnterEvents(eastl::vector<TriggerCharacterExitEvent>& outEvents)
{
    std::lock_guard<std::mutex> lock(m_enterMutex);

    outEvents.insert(outEvents.end(), m_enterQueue.begin(), m_enterQueue.end());
    m_enterQueue.clear();
}

CharacterControllerSystem::CharacterControllerSystem(Scene* scene, PhysicsSystem* physicsSystem)
    : m_scene(scene), m_physicsSystem(physicsSystem)
{
    m_characterContactListener = eastl::make_unique<SECharacterContactListener>();
    m_characterContactListener->SetBodyInterface(m_physicsSystem);
}

void CharacterControllerSystem::SetScene(Scene* scene)
{
    m_scene = scene;
}

void CharacterControllerSystem::SetPhysicsSystem(PhysicsSystem* physicsSystem)
{
    m_physicsSystem = physicsSystem;
    m_characterContactListener->SetBodyInterface(m_physicsSystem);
}

void CharacterControllerSystem::InitCharacters()
{
    for (auto& pair : m_scene->uuidToObjectMap)
    {
        auto gameObj = pair.second.get();
        if (gameObj->HasComponent<CharacterControllerComponent>())
        {
            auto charContrComp = gameObj->GetComponent<CharacterControllerComponent>();
            charContrComp->m_character->SetListener(m_characterContactListener.get());
        }
    }
}

void CharacterControllerSystem::UpdateCharacters(float deltaTime)
{
    for (auto& pair : m_scene->uuidToObjectMap)
    {
        UpdateCharacter(pair.second.get(), deltaTime);
    }
}

void CharacterControllerSystem::UpdateTriggerOverlaps() {
    eastl::vector<TriggerCharacterExitEvent> enterEvents;
    m_characterContactListener->FetchEnterEvents(enterEvents);
    for (const TriggerCharacterExitEvent& e : enterEvents)
    {
        auto triggerGO = Scene::GetInstance().GetGameObjectByUUID(e.Trigger);
        if (!triggerGO)
            continue;

        auto triggerComp = triggerGO->GetComponent<TriggerComponent>();
        if (!triggerComp)
            continue;

        triggerComp->OnEnter(e.Character);
    }

    eastl::vector<TriggerCharacterExitEvent> exitEvents;
    m_characterContactListener->FetchExitEvents(exitEvents);

    for (const TriggerCharacterExitEvent& e : exitEvents)
    {
        auto triggerGO = Scene::GetInstance().GetGameObjectByUUID(e.Trigger);
        if (!triggerGO)
            continue;

        auto triggerComp = triggerGO->GetComponent<TriggerComponent>();
        if (!triggerComp)
            continue;

        triggerComp->OnExit(e.Character);
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

    ApplyBounce(charComp, charContrComp);

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

    if ((input.x != 0 || input.y != 0) && controller->m_syncronizeYawWithCameraForwardDir)
    {
		float inputAngle = atan2(input.x, input.y);

        auto camera = Scene::GetInstance().m_cameraManager->GetCameraByUUID(controller->m_uuid);
        float camYaw = 0.0f;
        if (camera)
        {
            camYaw = camera->m_springArmParams.pitchYawRoll.y;
        }
        // character->m_yaw += camYaw;
        float desiredYaw = character->m_yaw + camYaw + inputAngle;
        if (desiredYaw > DX::XM_PI) { desiredYaw -= DX::XM_2PI; }
        else if (desiredYaw < -DX::XM_PI) { desiredYaw += DX::XM_2PI; }

        if (controller->m_inputVelocity == 0)
        {
            character->m_yaw = desiredYaw;
        }
        else
        {
            if ((character->m_yaw - desiredYaw) > DX::XM_PI)
            {
                character->m_yaw -= DX::XM_2PI;
            }
            else if ((character->m_yaw - desiredYaw) < -DX::XM_PI)
            {
                character->m_yaw += DX::XM_2PI;
            }

            character->m_yaw = std::lerp(
                character->m_yaw,
                desiredYaw,
                controller->m_turnAcceleration * deltaTime);
		}
        // character->m_yaw = std::clamp(character->m_yaw, -DX::XM_PI, DX::XM_PI);

        camera->m_springArmParams.pitchYawRoll.y = (desiredYaw - character->m_yaw - inputAngle);
        if (camera->m_springArmParams.pitchYawRoll.y > DX::XM_PI) { camera->m_springArmParams.pitchYawRoll.y -= DX::XM_2PI; }
        else if (camera->m_springArmParams.pitchYawRoll.y < -DX::XM_PI) { camera->m_springArmParams.pitchYawRoll.y += DX::XM_2PI; }
    }

    float desiredVelocity = input.Length() * controller->m_moveSpeed;

    float accel =
        controller->m_grounded
        ? controller->m_acceleration
        : controller->m_airAcceleration;

    controller->m_inputVelocity = std::lerp(
        controller->m_inputVelocity,
        desiredVelocity,
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

void CharacterControllerSystem::ApplyBounce(
    eastl::shared_ptr<CharacterComponent> character,
    eastl::shared_ptr<CharacterControllerComponent> controller)
{
    if (!character->m_bounced)
        return;

    if (controller->m_velocity.Dot(character->m_bounceNormal) <= 0.0f)
    {
        if (controller->m_velocity.Dot(character->m_bounceNormal) >= -character->m_bounceSpeed)
        {
            controller->m_velocity += character->m_bounceNormal * (-controller->m_velocity.Dot(character->m_bounceNormal) + character->m_bounceSpeed);
        }
        else
        {
            controller->m_velocity = DXSM::Vector3::Reflect(controller->m_velocity, character->m_bounceNormal);
        }

        controller->m_grounded = false;

        character->m_bounced = false;
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
            controller->m_velocity.x + controller->m_inputVelocity * sin(character->m_yaw),
            controller->m_velocity.y,
            controller->m_velocity.z + controller->m_inputVelocity * cos(character->m_yaw)
        ) + controller->m_groundSpeed
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

    JPH::PhysicsSystem& physSystem = m_physicsSystem->GetWorld();

    controller->m_character->ExtendedUpdate(
        deltaTime,
        JPH::Vec3(0.0f, controller->m_gravity, 0.0f), update_settings,
        physSystem.GetDefaultBroadPhaseLayerFilter(SE::Layers::MOVING), // SE::Layers
        physSystem.GetDefaultLayerFilter(SE::Layers::MOVING), // SE::Layers
        {},
        {},
        m_physicsSystem->GetAllocator()
    );
}

void CharacterControllerSystem::UpdateGroundState(
    eastl::shared_ptr<CharacterComponent> character,
    eastl::shared_ptr<CharacterControllerComponent> controller)
{
    bool wasGrounded = controller->m_grounded;

    controller->m_grounded =
        controller->m_character->GetGroundState() ==
        JPH::CharacterBase::EGroundState::OnGround;

    if (controller->m_grounded)
    {
        auto movingPlatformUUID = SE::UUID(controller->m_character->GetGroundUserData());
        /*
        JPH::PhysicsSystem& physSystem = m_physicsSystem->GetWorld();
        physSystem.GetBodyInterface().GetLinearVelocity(;
        */
        auto platform = m_physicsSystem->GetMovingPlatform(movingPlatformUUID);
        if (platform && platform->m_affectCharacters)
        {
            controller->m_groundSpeed = platform->m_velocity;
        }
        else
        {
            controller->m_groundSpeed = JPH::Vec3::sZero();
        }

        if (!wasGrounded)
        {
            controller->m_velocity.x = 0;
            controller->m_velocity.z = 0;
        }
    }
    else
    {
        if (!wasGrounded)
        {
            controller->m_groundSpeed = JPH::Vec3::sZero();
        }
    }

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
