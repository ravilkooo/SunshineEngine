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

    {
        std::lock_guard<std::mutex> lock(m_enterMutex);
        m_enterQueue.push_back(ev);
    }

    TriggerOverlapKey key;
    key.TriggerBody = triggerBody.GetID();
    key.OtherBody = otherBody.GetID();

    {
        std::lock_guard<std::mutex> lock(m_exitMutex);
        m_activeOverlaps[key] = ev;
    }
}

void TriggerContactListener::OnContactRemoved(
    const JPH::SubShapeIDPair& inSubShapePair)
{
    const JPH::BodyID a = inSubShapePair.GetBody1ID();
    const JPH::BodyID b = inSubShapePair.GetBody2ID();

    std::lock_guard<std::mutex> lock(m_exitMutex);

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
    std::lock_guard<std::mutex> lock(m_exitMutex);

    outEvents.insert(outEvents.end(), m_exitQueue.begin(), m_exitQueue.end());
    m_exitQueue.clear();
}

void TriggerContactListener::FetchEnterEvents(eastl::vector<TriggerExitEvent>& outEvents)
{
    std::lock_guard<std::mutex> lock(m_enterMutex);

    outEvents.insert(outEvents.end(), m_enterQueue.begin(), m_enterQueue.end());
    m_enterQueue.clear();
}

bool TriggerContactListener::IsTrigger(const JPH::Body& body) const
{
    return body.IsSensor();
}