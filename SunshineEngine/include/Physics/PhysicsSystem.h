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
#include <Component/PhysicsComponent.h>
#include <Utils/UUID.h>


class PhysicsSystem_Info {
public:
    eastl::vector<eastl::string> m_collisionLayerGroups;
    eastl::vector<eastl::string> m_collisionLayers;

    SE::CollisionLayerVsLayerTable m_collisionLayerVsLayerTable;
    SE::CollisionLayerVsGroupTable m_collisionLayerVsGroupTable;

    eastl::unordered_map<SE::CollisionLayer, SE::CollisionGroup> m_layerToGroupMapping;
};


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

// An example contact listener
class MyContactListener : public JPH::ContactListener
{
public:
    // See: ContactListener
    virtual JPH::ValidateResult	OnContactValidate(
        const JPH::Body& inBody1, const JPH::Body& inBody2,
        JPH::RVec3Arg inBaseOffset,
        const JPH::CollideShapeResult& inCollisionResult) override
    {
        //std::cout << "Contact validate callback" << std::endl;

        // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    virtual void OnContactAdded(
        const JPH::Body& inBody1, const JPH::Body& inBody2,
        const JPH::ContactManifold& inManifold,
        JPH::ContactSettings& ioSettings) override
    {
        //std::cout << "A contact was added" << std::endl;
    }

    virtual void OnContactPersisted(
        const JPH::Body& inBody1,
        const JPH::Body& inBody2,
        const JPH::ContactManifold& inManifold,
        JPH::ContactSettings& ioSettings) override
    {
        //std::cout << "A contact was persisted" << std::endl;
    }

    virtual void OnContactRemoved(
        const JPH::SubShapeIDPair& inSubShapePair) override
    {
        //std::cout << "A contact was removed" << std::endl;
    }
};

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
            return inObject2 == SE::Layers::MOVING; // Non moving only collides with moving
        case SE::Layers::MOVING:
            return true; // Moving collides with everything
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
        case (JPH::BroadPhaseLayer::Type)SE::BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)SE::BroadPhaseLayers::MOVING:		return "MOVING";
        default:													JPH_ASSERT(false); return "INVALID";
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
            return true;
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
    
    void CreateAndAddBody(PhysicsComponent* physComp);

    // Add objects before this step
    void FinalizeScene();

    void SyncronizeTransforms(Scene* scene);

    void Step(float dt);

    JPH::PhysicsSystem& GetWorld();
    JPH::BodyInterface& Bodies();

    bool IsValid() { return m_isValid; }

private:

    void ClearAllBodies();

    eastl::unique_ptr<BPLayerInterfaceImpl> m_bpInterface; // { 2, 2 };
    eastl::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> m_objectVsBpFilter; // ; // { 2 };
    eastl::unique_ptr<ObjectLayerPairFilterImpl> m_objectPairFilter; // { 2 };

    eastl::unique_ptr<JPH::JobSystemThreadPool> m_jobSystem;
    eastl::unique_ptr<JPH::TempAllocatorImpl> m_tempAllocator;
    eastl::unique_ptr<JPH::PhysicsSystem> m_physicsSystem;
    JPH::BodyInterface* m_bodyInterface = nullptr;
    MyBodyActivationListener m_bodyActivationListener;
    MyContactListener m_contactListener;

    eastl::vector<PhysicsBodyEntry> m_bodyEntries;

    bool m_isValid = false;
    // TO-DO
    // Something like list of all phys objects (or some entries like in nau engine)
    // It will be used for (Syn Step) syncronization with TransformComponent and other stuff
    // D:\Workspace\NauFork\NauEnginePublic\engine\core\modules\physics\src\physics_service.cpp

    // You can make PhysicsBodyEntry like in nau
    // D:\Workspace\NauFork\NauEnginePublic\engine\core\modules\physics\src\physics_world_state.h
};

