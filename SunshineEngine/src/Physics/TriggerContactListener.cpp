#include <Physics/TriggerContactListener.h>
#include <Scene.h>
#include <GameObject/GameObject.h>
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

    const bool b1Trigger = inBody1.IsSensor();
    const bool b2Trigger = inBody2.IsSensor();

    if (!b1Trigger && !b2Trigger)
        return;

    if (b1Trigger && b2Trigger)
        return;

    const JPH::Body& triggerBody = b1Trigger ? inBody1 : inBody2;
    const JPH::Body& otherBody = b1Trigger ? inBody2 : inBody1;

    TriggerExitEvent ev;
    ev.Trigger = SE::UUID((uint64_t)triggerBody.GetUserData());
    ev.Other = SE::UUID((uint64_t)otherBody.GetUserData());

    TriggerOverlapKey key;
    key.TriggerBody = triggerBody.GetID();
    key.OtherBody = otherBody.GetID();

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_activeOverlaps[key] = ev;
    }
}

void TriggerContactListener::OnContactRemoved(
    const JPH::SubShapeIDPair& inSubShapePair)
{
    const JPH::BodyID a = inSubShapePair.GetBody1ID();
    const JPH::BodyID b = inSubShapePair.GetBody2ID();

    std::lock_guard<std::mutex> lock(m_mutex);

    // пробуем оба направления (мы не знаем кто trigger)
    TriggerOverlapKey key1{ a, b };
    TriggerOverlapKey key2{ b, a };

    auto it = m_activeOverlaps.find(key1);
    if (it == m_activeOverlaps.end())
        it = m_activeOverlaps.find(key2);

    if (it == m_activeOverlaps.end())
        return;

    // кидаем exit-событие
    m_exitQueue.push_back(it->second);

    // удаляем активное пересечение
    m_activeOverlaps.erase(it);
}

void TriggerContactListener::FetchExitEvents(eastl::vector<TriggerExitEvent>& outEvents)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    outEvents.insert(outEvents.end(), m_exitQueue.begin(), m_exitQueue.end());
    m_exitQueue.clear();
}

void TriggerContactListener::HandleTriggerContact(
    const JPH::Body& body1,
    const JPH::Body& body2,
    bool entered)
{
    SE::UUID uuid1 = SE::UUID((std::uint64_t)body1.GetUserData());
    SE::UUID uuid2 = SE::UUID((std::uint64_t)body2.GetUserData());

    if (uuid1 == uuid2)
        return;

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