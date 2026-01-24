#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>

#include <Utils/UUID.h>

class Scene;

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

private:
    void HandleTriggerContact(const JPH::Body& body1, const JPH::Body& body2, bool entered);
    bool IsTrigger(const JPH::Body& body) const;
};