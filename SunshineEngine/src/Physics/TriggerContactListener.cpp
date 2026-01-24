#include <Physics/TriggerContactListener.h>
#include <Scene.h>
#include <Component/TriggerComponent.h>
#include <Component/PhysicsComponent.h>

JPH::ValidateResult TriggerContactListener::OnContactValidate(
    const JPH::Body& inBody1,
    const JPH::Body& inBody2,
    JPH::RVec3Arg inBaseOffset,
    const JPH::CollideShapeResult& inCollisionResult)
{
    // Allow all trigger contacts
    return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void TriggerContactListener::OnContactAdded(
    const JPH::Body& inBody1,
    const JPH::Body& inBody2,
    const JPH::ContactManifold& inManifold,
    JPH::ContactSettings& ioSettings)
{
    HandleTriggerContact(inBody1, inBody2, true);  // Enter
}

void TriggerContactListener::OnContactRemoved(
    const JPH::SubShapeIDPair& inSubShapePair)
{
    // Note: We don't have body references here, so exit handling
    // is done via OverlapQuery in PhysicsSystem::Step()
}

void TriggerContactListener::HandleTriggerContact(
    const JPH::Body& body1,
    const JPH::Body& body2,
    bool entered)
{
    SE::UUID uuid1 = SE::UUID((std::uint64_t)body1.GetUserData());
    SE::UUID uuid2 = SE::UUID((std::uint64_t)body2.GetUserData());

    auto go1 = Scene::GetInstance().GetGameObjectByUUID(uuid1);
    auto go2 = Scene::GetInstance().GetGameObjectByUUID(uuid2);

    if (!go1 || !go2)
        return;

    // Check if body1 is trigger
    auto trigger1 = go1->GetComponent<TriggerComponent>();
    if (trigger1 && entered)
    {
        trigger1->OnEnter(uuid2);
    }

    // Check if body2 is trigger
    auto trigger2 = go2->GetComponent<TriggerComponent>();
    if (trigger2 && entered)
    {
        trigger2->OnEnter(uuid1);
    }
}

bool TriggerContactListener::IsTrigger(const JPH::Body& body) const
{
    return body.IsSensor();
}