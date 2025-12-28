#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>

#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_set.h>
#include <EASTL/unordered_map.h>

#include <wrl.h>

#include <TracingSystem/TracedBody.h>

#include <Utils/UUID.h>

/*

class SingleLayerFilter : public JPH::ObjectLayerFilter
{
public:
    explicit SingleLayerFilter(JPH::ObjectLayer layer) : m_layer(layer) {}

    bool ShouldCollide(JPH::ObjectLayer layer) const override
    {
        return layer == m_layer;
    }

private:
    JPH::ObjectLayer m_layer;
};

class IgnoreUUIDBodyFilter : public JPH::BodyFilter
{
public:
    IgnoreUUIDBodyFilter(const eastl::unordered_set<SE::UUID>& ignore,
        const JPH::BodyLockInterface& body_lock)
        : m_ignore(ignore), m_bodyLock(body_lock) {
    }

    bool ShouldCollide(const JPH::BodyID& body_id) const override
    {
        JPH::BodyLockRead lock(m_bodyLock, body_id);
        if (!lock.Succeeded())
            return false;

        const JPH::Body& body = lock.GetBody();
        SE::UUID id = *reinterpret_cast<const SE::UUID*>(body.GetUserData());

        return m_ignore.find(id) == m_ignore.end();
    }

private:
    const eastl::unordered_set<SE::UUID>& m_ignore;
    const JPH::BodyLockInterface& m_bodyLock;
};

class Scene;

class TracingSystem
{
public:
    // Use external physics system (existing behaviour)
    explicit TracingSystem(JPH::PhysicsSystem* physics)
        : m_physics(physics), m_ownsPhysics(false) {
    }

    // Construct an independent internal Jolt PhysicsSystem. This will
    // register the required Jolt runtime, allocate a temp allocator and
    // job system and initialize an internal PhysicsSystem instance.
    TracingSystem();

    ~TracingSystem();


    void CreateAndAddBody(TracedBody* tracedBody);

    // Add objects before this step
    void FinalizeScene();

    void SyncronizeTransforms(Scene* scene);

    void ClearAllBodies();

    JPH::BodyInterface& Bodies() { return *m_bodyInterface; }


private:
    // If this object created its own physics system this will hold it
    // and we will also own the temp allocator / job system instances
    // required by Jolt.
    JPH::PhysicsSystem* m_physics = nullptr;
    JPH::BodyInterface* m_bodyInterface = nullptr;
    bool m_ownsPhysics = false;

    eastl::unique_ptr<JPH::JobSystemThreadPool> m_jobSystem;
    eastl::unique_ptr<JPH::TempAllocatorImpl> m_tempAllocator;
    eastl::unique_ptr<JPH::PhysicsSystem> m_physicsSystem;
    eastl::unique_ptr<JPH::Factory> m_factory; // optional ownership helper
    // keep these alive while our physics system exists (Init needs references)
    eastl::unique_ptr<JPH::BroadPhaseLayerInterface> m_bpInterface;
    eastl::unique_ptr<JPH::ObjectVsBroadPhaseLayerFilter> m_objectVsBpFilter;
    eastl::unique_ptr<JPH::ObjectLayerPairFilter> m_objectPairFilter;

    eastl::vector<JPH::BodyID> m_bodyEntries;
};
*/
