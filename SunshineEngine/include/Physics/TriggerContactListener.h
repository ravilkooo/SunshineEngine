#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>

#include <Utils/UUID.h>

#include <EASTL/vector.h>
#include <mutex>

class Scene;

struct TriggerExitEvent
{
    SE::UUID Trigger;
    SE::UUID Other;
};

struct TriggerOverlapKey
{
    JPH::BodyID TriggerBody;
    JPH::BodyID OtherBody;

    bool operator==(const TriggerOverlapKey& rhs) const
    {
        return TriggerBody == rhs.TriggerBody && OtherBody == rhs.OtherBody;
    }
};

struct TriggerOverlapKeyHash
{
    size_t operator()(const TriggerOverlapKey& k) const noexcept
    {
        // простая комбинированная хеш-функция
        size_t h1 = (size_t)k.TriggerBody.GetIndexAndSequenceNumber();
        size_t h2 = (size_t)k.OtherBody.GetIndexAndSequenceNumber();
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ull + (h1 << 6) + (h1 >> 2));
    }
};

class TriggerContactListener : public JPH::ContactListener
{
public:
    TriggerContactListener() {}

    virtual JPH::ValidateResult OnContactValidate(
        const JPH::Body& inBody1,
        const JPH::Body& inBody2,
        JPH::RVec3Arg inBaseOffset,
        const JPH::CollideShapeResult& inCollisionResult) override;

    virtual void OnContactAdded(
        const JPH::Body& inBody1,
        const JPH::Body& inBody2,
        const JPH::ContactManifold& inManifold,
        JPH::ContactSettings& ioSettings) override;

    virtual void OnContactRemoved(
        const JPH::SubShapeIDPair& inSubShapePair) override;

    //void FetchRemovedPairs(eastl::vector<JPH::SubShapeIDPair>& outPairs);

private:
    void HandleTriggerContact(const JPH::Body& body1, const JPH::Body& body2, bool entered);
    bool IsTrigger(const JPH::Body& body) const;

public:
    void FetchExitEvents(eastl::vector<TriggerExitEvent>& outEvents);
    void FetchEnterEvents(eastl::vector<TriggerExitEvent>& outEvents);

private:
    std::mutex m_exitMutex;
    std::mutex m_enterMutex;

    std::unordered_map<TriggerOverlapKey, TriggerExitEvent, TriggerOverlapKeyHash> m_activeOverlaps;
    
    eastl::vector<TriggerExitEvent> m_enterQueue;

    eastl::vector<TriggerExitEvent> m_exitQueue;



};