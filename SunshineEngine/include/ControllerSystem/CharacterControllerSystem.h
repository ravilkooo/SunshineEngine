#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

#include "GameObject/GameObject.h"

class Scene;
class CharacterComponent;
class CharacterControllerComponent;

struct TriggerCharacterExitEvent
{
    SE::UUID Trigger;
    SE::UUID Character;
};

struct TriggerCharacterOverlapKey
{
    JPH::BodyID TriggerBody;
    JPH::CharacterID Character;

    bool operator==(const TriggerCharacterOverlapKey& rhs) const
    {
        return TriggerBody == rhs.TriggerBody && Character == rhs.Character;
    }
};

struct TriggerCharacterOverlapKeyHash
{
    size_t operator()(const TriggerCharacterOverlapKey& k) const noexcept
    {
        // простая комбинированная хеш-функция
        size_t h1 = (size_t)k.TriggerBody.GetIndexAndSequenceNumber();
        size_t h2 = (size_t)k.Character.GetHash();
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ull + (h1 << 6) + (h1 >> 2));
    }
};

class SECharacterContactListener : public JPH::CharacterContactListener
{
public:
    bool OnContactValidate(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2) override;

    void OnContactAdded(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2,
        JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings) override;

    void OnContactRemoved(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2) override;

    void SetBodyInterface(PhysicsSystem* physicsSystem);

    void FetchExitEvents(eastl::vector<TriggerCharacterExitEvent>& outEvents);
    void FetchEnterEvents(eastl::vector<TriggerCharacterExitEvent>& outEvents);
private:
    JPH::BodyInterface* m_bodyInterface = nullptr;
    PhysicsSystem* m_physicsSystem = nullptr;

    std::mutex m_enterMutex;
    std::mutex m_exitMutex;

    eastl::vector<TriggerCharacterExitEvent> m_enterQueue;
    eastl::vector<TriggerCharacterExitEvent> m_exitQueue;

    std::unordered_map<TriggerCharacterOverlapKey, TriggerCharacterExitEvent, TriggerCharacterOverlapKeyHash> m_activeOverlaps;
};

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
    void InitCharacters();

    //
    // Update
    //

    void Update(float deltaTime);
    void UpdateTriggerOverlaps();

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
    PhysicsSystem* m_physicsSystem = nullptr;
    eastl::unique_ptr<SECharacterContactListener> m_characterContactListener;
};