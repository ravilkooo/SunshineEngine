#pragma once

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

#include "GameObject/GameObject.h"

class Scene;
class CharacterComponent;
class CharacterControllerComponent;

// Connect CharacterComponent and CharacterControllerComponent
// Requested inputs (control) from Character -> Character movement
class CharacterControllerSystem
{
public:

    CharacterControllerSystem() = default;

    explicit CharacterControllerSystem(Scene* scene, PhysicsSystem* physicsSystem);


    //
    // Scene
    //

    void SetScene(Scene* scene);
    void SetPhysicsSystem(PhysicsSystem* physicsSystem);


    //
    // Update
    //

    void Update(float deltaTime);


private:

    //
    // Internal update stages
    //

    void UpdateCharacter(GameObject* gameObj, float deltaTime);

    void ApplyMovementInput(
        eastl::shared_ptr<CharacterComponent> character,
        eastl::shared_ptr<CharacterControllerComponent> controller,
        float deltaTime
    );

    void ApplyGravity(
        eastl::shared_ptr<CharacterComponent> character,
        eastl::shared_ptr<CharacterControllerComponent> controller,
        float deltaTime
    );

    void ApplyJump(
        eastl::shared_ptr<CharacterComponent> character,
        eastl::shared_ptr<CharacterControllerComponent> controller
    );

    void UpdatePhysics(
        eastl::shared_ptr<CharacterComponent> character,
        eastl::shared_ptr<CharacterControllerComponent> controller,
        float deltaTime
    );

    void UpdateGroundState(
        eastl::shared_ptr<CharacterComponent> character,
        eastl::shared_ptr<CharacterControllerComponent> controller
    );

    void ClearFrameState(
        eastl::shared_ptr<CharacterComponent> character,
        eastl::shared_ptr<CharacterControllerComponent> controller
    );

    void SynchronizeTransforms(GameObject* gameObj);

private:
    Scene* m_scene = nullptr;
    PhysicsSystem* m_PhysicsSystem = nullptr;
};