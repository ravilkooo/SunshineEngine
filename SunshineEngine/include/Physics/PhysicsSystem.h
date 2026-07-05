#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>

#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h>
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_set.h>
#include <EASTL/unordered_map.h>
#include <EASTL/string.h>

#include <wrl.h>

#include <iostream>

#include <Physics/CollisionUtils.h>
#include <Physics/CollisionLayerVsLayerTable.h>
#include <Physics/CollisionLayerVsGroupTable.h>
#include <Physics/TriggerContactListener.h>

#include <EASTL/unordered_map.h>


#include <Utils/UUID.h>

class PhysicsComponent;
class TriggerComponent;
class MovingPlatformComponent;

class PhysicsSystem_Info {
public:
    eastl::vector<eastl::string> m_collisionLayerGroups;
    eastl::vector<eastl::string> m_collisionLayers;

    SE::CollisionLayerVsLayerTable m_collisionLayerVsLayerTable;
    SE::CollisionLayerVsGroupTable m_collisionLayerVsGroupTable;

    eastl::unordered_map<SE::CollisionLayer, SE::CollisionGroup> m_layerToGroupMapping;
};

//////////////////////////////////////////
// FOR TRACING ONLY (GAI)
//////////////////////////////////////////

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

class EmptyFilter : public JPH::ObjectLayerFilter
{
public:
    explicit EmptyFilter() {}

    bool ShouldCollide(JPH::ObjectLayer layer) const override
    {
        return true;
    }
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
        SE::UUID id = SE::UUID(body.GetUserData());

        return m_ignore.find(id) == m_ignore.end();
    }

private:
    const eastl::unordered_set<SE::UUID>& m_ignore;
    const JPH::BodyLockInterface& m_bodyLock;
};
//////////////////////////////////////////
//////////////////////////////////////////

static void TraceImpl(const char* inFMT, ...)
{
    // Format the message
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);

    // Print to the TTY
    std::cout << buffer << std::endl;
}

// An example activation listener
class MyBodyActivationListener : public JPH::BodyActivationListener
{
public:
    virtual void OnBodyActivated(const JPH::BodyID& inBodyID,
        JPH::uint64 inBodyUserData) override
    {
        //std::cout << "A body got activated" << std::endl;
    }

    virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID,
        JPH::uint64 inBodyUserData) override
    {
        //std::cout << "A body went to sleep" << std::endl;
    }
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(
        JPH::ObjectLayer inObject1,
        JPH::ObjectLayer inObject2) const override
    {
        switch (inObject1)
        {
        case SE::Layers::NON_MOVING:
            // Non-moving collides with moving and character
            return inObject2 == SE::Layers::MOVING ||
                inObject2 == SE::Layers::CHARACTER;

        case SE::Layers::MOVING:
            // Moving collides with non-moving, moving and character
            return inObject2 == SE::Layers::NON_MOVING ||
                inObject2 == SE::Layers::MOVING ||
                inObject2 == SE::Layers::CHARACTER ||
                inObject2 == SE::Layers::TRIGGER;

        case SE::Layers::TRIGGER:
            return inObject2 == SE::Layers::CHARACTER ||
                inObject2 == SE::Layers::MOVING;


        case SE::Layers::CHARACTER:
            // Moving character with non-moving, moving and character
            return inObject2 == SE::Layers::NON_MOVING ||
                inObject2 == SE::Layers::MOVING ||
                inObject2 == SE::Layers::CHARACTER ||
                inObject2 == SE::Layers::TRIGGER;

        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl()
    {
        // Create a mapping table from object to broad phase layer
        mObjectToBroadPhase[SE::Layers::NON_MOVING] = SE::BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[SE::Layers::MOVING] = SE::BroadPhaseLayers::MOVING;
        mObjectToBroadPhase[SE::Layers::TRIGGER] = SE::BroadPhaseLayers::TRIGGER;
        mObjectToBroadPhase[SE::Layers::CHARACTER] = SE::BroadPhaseLayers::MOVING;
    }

    virtual UINT GetNumBroadPhaseLayers() const override
    {
        return SE::BroadPhaseLayers::NUM_LAYERS;
    }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
    {
        JPH_ASSERT(inLayer < SE::Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
    {
        switch ((JPH::BroadPhaseLayer::Type)inLayer)
        {
        case (JPH::BroadPhaseLayer::Type)SE::BroadPhaseLayers::NON_MOVING:  return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)SE::BroadPhaseLayers::MOVING:      return "MOVING";
        case (JPH::BroadPhaseLayer::Type)SE::BroadPhaseLayers::TRIGGER:     return "TRIGGER";
        default:                                                             JPH_ASSERT(false); return "INVALID";
        }
    }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[SE::Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(
        JPH::ObjectLayer inLayer1,
        JPH::BroadPhaseLayer inLayer2) const override
    {
        switch (inLayer1)
        {
        case SE::Layers::NON_MOVING:
            return inLayer2 == SE::BroadPhaseLayers::MOVING;

        case SE::Layers::MOVING:
            return inLayer2 == SE::BroadPhaseLayers::NON_MOVING || 
                   inLayer2 == SE::BroadPhaseLayers::MOVING ||
                inLayer2 == SE::BroadPhaseLayers::TRIGGER;

        case SE::Layers::TRIGGER:
            return inLayer2 == SE::BroadPhaseLayers::TRIGGER ||
                inLayer2 == SE::BroadPhaseLayers::MOVING;

        case SE::Layers::CHARACTER:
            return inLayer2 == SE::BroadPhaseLayers::NON_MOVING ||
                inLayer2 == SE::BroadPhaseLayers::MOVING ||
                inLayer2 == SE::BroadPhaseLayers::TRIGGER;

        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

class PhysicsBodyEntry {
public:
    JPH::BodyID m_joltBodyId;
    //SE::UUID m_objectUUID;

    PhysicsBodyEntry(JPH::BodyID bodyId) :
        m_joltBodyId(bodyId)
        //m_objectUUID(objectUUID)
    {}
};

class Scene;

class PhysicsSystem
{
public:
    PhysicsSystem();
    ~PhysicsSystem();

    // begin: world-space start; dir: normalized; layerKey: your editor layer key
    bool PerceptionTrace(const JPH::RVec3& begin,
        const JPH::Vec3& dir,
        float length,
        const eastl::vector<SE::UUID>& ignore,
        SE::UUID* out_id);
    
    void CreateAndAddBody(PhysicsComponent* physComp);

    // Remove and destroy a specific physics body
    void RemoveBody(PhysicsComponent* physComp);

    // Add objects before this step
    void FinalizeScene();

    void SyncronizeTransforms(Scene* scene, float deltaTime);

    void Step(float dt);

    JPH::PhysicsSystem& GetWorld();
    JPH::TempAllocatorImpl& GetAllocator();
    JPH::BodyInterface& Bodies();

    bool IsValid() { return m_isValid; }

    // Create a trigger
    void CreateAndAddTrigger(TriggerComponent* triggerComp);

    // Remove and destroy a specific physics body
    void RemoveTrigger(TriggerComponent* triggerComp);

    // Check trigger overlaps each frame
    void UpdateTriggerOverlaps();

    // Global gravity value
    void SetGravity(DXSM::Vector3 inGravity);
    DXSM::Vector3 GetGravity();

    void AddMovingPlatform(MovingPlatformComponent* platformComp);
    MovingPlatformComponent* GetMovingPlatform(SE::UUID platformUUID);

private:

    void ClearAllBodies();

    eastl::unique_ptr<BPLayerInterfaceImpl> m_bpInterface; // { 2, 2 };
    eastl::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> m_objectVsBpFilter; // { 2 };
    eastl::unique_ptr<ObjectLayerPairFilterImpl> m_objectPairFilter; // { 2 };

    eastl::unique_ptr<JPH::JobSystemThreadPool> m_jobSystem;
    eastl::unique_ptr<JPH::TempAllocatorImpl> m_tempAllocator;
    eastl::unique_ptr<JPH::PhysicsSystem> m_physicsSystem;
    JPH::BodyInterface* m_bodyInterface = nullptr;
    MyBodyActivationListener m_bodyActivationListener;

    eastl::vector<PhysicsBodyEntry> m_bodyEntries;

    TriggerContactListener m_triggerContactListener;
    eastl::unordered_map<SE::UUID, TriggerComponent*> m_activeTriggers;

    eastl::unordered_map<SE::UUID, MovingPlatformComponent*> m_movingPlatforms;

    bool m_isValid = false;
    
public:
    void EnqueueCommand(std::function<void()> fn);

    void FlushCommands(); // вызвать в безопасной точке

    void EnqueuePreNextFrameCommand(std::function<void()> fn);

    void FlushPreNextFrameCommands(); // вызвать в безопасной точке
private:

    std::mutex m_cmdMutex;
    std::vector<std::function<void()>> m_cmds;

    std::mutex m_preNextFrameCmdMutex;
    std::vector<std::function<void()>> m_preNextFrameCmds;
};

