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

#include <wrl.h>

#include <iostream>

namespace Layers
{
    static constexpr JPH::ObjectLayer NON_MOVING = 0;
    static constexpr JPH::ObjectLayer MOVING = 1;
    static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

namespace BroadPhaseLayers
{
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr UINT NUM_LAYERS(2);
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
        std::cout << "Contact validate callback" << std::endl;

        // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    virtual void OnContactAdded(
        const JPH::Body& inBody1, const JPH::Body& inBody2,
        const JPH::ContactManifold& inManifold,
        JPH::ContactSettings& ioSettings) override
    {
        std::cout << "A contact was added" << std::endl;
    }

    virtual void OnContactPersisted(
        const JPH::Body& inBody1,
        const JPH::Body& inBody2,
        const JPH::ContactManifold& inManifold,
        JPH::ContactSettings& ioSettings) override
    {
        std::cout << "A contact was persisted" << std::endl;
    }

    virtual void OnContactRemoved(
        const JPH::SubShapeIDPair& inSubShapePair) override
    {
        std::cout << "A contact was removed" << std::endl;
    }
};

// An example activation listener
class MyBodyActivationListener : public JPH::BodyActivationListener
{
public:
    virtual void OnBodyActivated(const JPH::BodyID& inBodyID,
        JPH::uint64 inBodyUserData) override
    {
        std::cout << "A body got activated" << std::endl;
    }

    virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID,
        JPH::uint64 inBodyUserData) override
    {
        std::cout << "A body went to sleep" << std::endl;
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
        case Layers::NON_MOVING:
            return inObject2 == Layers::MOVING; // Non moving only collides with moving
        case Layers::MOVING:
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
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    virtual UINT GetNumBroadPhaseLayers() const override
    {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
    {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
    {
        switch ((JPH::BroadPhaseLayer::Type)inLayer)
        {
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
        default:													JPH_ASSERT(false); return "INVALID";
        }
    }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
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
        case Layers::NON_MOVING:
            return inLayer2 == BroadPhaseLayers::MOVING;
        case Layers::MOVING:
            return true;
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

class PhysicsSystem
{
public:
    PhysicsSystem() {
        JPH::RegisterDefaultAllocator();
        //JPH::Trace = [](const char* fmt, ...) {}; // hook your logger
        JPH::Trace = TraceImpl;

        JPH::Factory::sInstance = new JPH::Factory();

        JPH::RegisterTypes();

        //tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
#ifdef JPH_DISABLE_TEMP_ALLOCATOR
        tempAllocator = new TempAllocatorMalloc();
#else
        tempAllocator = eastl::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024); // <-- ERROR
#endif

        jobSystem = eastl::make_unique<JPH::JobSystemThreadPool>(
            JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1
        );

        // bpInterface = eastl::make_unique<JPH::BroadPhaseLayerInterfaceTable>(
        //     2, 2);
        // objectVsBpFilter = eastl::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
        // objectPairFilter = eastl::make_unique<JPH::ObjectLayerPairFilterTable>(2);

        bpInterface = eastl::make_unique<BPLayerInterfaceImpl>();
        objectVsBpFilter = eastl::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
        objectPairFilter = eastl::make_unique<ObjectLayerPairFilterImpl>();

        physicsSystem = eastl::make_unique<JPH::PhysicsSystem>();
        const UINT maxBodies = 8192, numBodyMutexes = 0, maxBodyPairs = 8192, maxContactConstraints = 8192;
        physicsSystem->Init(
            maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints,
            *bpInterface, *objectVsBpFilter, *objectPairFilter);
        physicsSystem->SetBodyActivationListener(&body_activation_listener);
        physicsSystem->SetContactListener(&contact_listener);
        physicsSystem->SetGravity(JPH::Vec3(0, -9.81f, 0));
        bodyInterface = &physicsSystem->GetBodyInterface();
    }

    void AddSimpleScene() {
        // Next we can create a rigid body to serve as the floor, we make a large box
        // Create the settings for the collision volume (the shape).
        // Note that for simple shapes (like boxes) you can also directly construct a BoxShape.
        JPH::BoxShapeSettings floor_shape_settings(JPH::Vec3(100.0f, 0.1f, 100.0f));
        floor_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

        // Create the shape
        JPH::ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
        JPH::ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

        // Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
        JPH::BodyCreationSettings floor_settings(
            floor_shape, JPH::RVec3(0.0, -5.0, 0.0),
            JPH::Quat::sIdentity(), JPH::EMotionType::Static,
            Layers::NON_MOVING);

        // Create the actual rigid body
        floor = bodyInterface->CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr

        // Add it to the world
        bodyInterface->AddBody(floor->GetID(), JPH::EActivation::DontActivate);

        // Now create a dynamic body to bounce on the floor
        // Note that this uses the shorthand version of creating and adding a body to the world
        JPH::BodyCreationSettings sphere_settings(new JPH::SphereShape(0.5f), JPH::RVec3(0.0, 2.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING);
        sphere_id = bodyInterface->CreateAndAddBody(sphere_settings, JPH::EActivation::Activate);

        // Now you can interact with the dynamic body, in this case we're going to give it a velocity.
        // (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
        bodyInterface->SetLinearVelocity(sphere_id, JPH::Vec3(0.0f, -5.0f, 0.0f));

        physicsSystem->OptimizeBroadPhase();
    }

    void Step(float dt) {
        // Output current position and velocity of the sphere
        JPH::RVec3 position = bodyInterface->GetCenterOfMassPosition(sphere_id);
        JPH::Vec3 velocity = bodyInterface->GetLinearVelocity(sphere_id);
        std::cout << " :: Position = (" << position.GetX() << ", " << position.GetY() << ", " << position.GetZ() << "), Velocity = (" << velocity.GetX() << ", " << velocity.GetY() << ", " << velocity.GetZ() << ")" << std::endl;

        physicsSystem->Update(
            dt, /*collisionSteps*/1,
            // /*integrationSubSteps*/ 1,
            tempAllocator.get(), jobSystem.get());
    }

    void RemoveSimpleScene() {
        // Remove the sphere from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
        bodyInterface->RemoveBody(sphere_id);

        // Destroy the sphere. After this the sphere ID is no longer valid.
        bodyInterface->DestroyBody(sphere_id);

        // Remove and destroy the floor
        bodyInterface->RemoveBody(floor->GetID());
        bodyInterface->DestroyBody(floor->GetID());

        // Unregisters all types with the factory and cleans up the default material
        JPH::UnregisterTypes();

        // Destroy the factory
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }

    JPH::RVec3 SpherePosition() {
        return bodyInterface->GetCenterOfMassPosition(sphere_id);
    }

    JPH::PhysicsSystem& GetWorld() { return *physicsSystem; }
    JPH::BodyInterface& Bodies() { return *bodyInterface; }

private:
    // Implement layers/filters shown in the next section before Init
    eastl::unique_ptr<BPLayerInterfaceImpl> bpInterface; // { 2, 2 };
    eastl::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> objectVsBpFilter; // ; // { 2 };
    eastl::unique_ptr<ObjectLayerPairFilterImpl> objectPairFilter; // { 2 };

    eastl::unique_ptr<JPH::JobSystemThreadPool> jobSystem;
    eastl::unique_ptr<JPH::TempAllocatorImpl> tempAllocator;
    eastl::unique_ptr<JPH::PhysicsSystem> physicsSystem;
    JPH::BodyInterface* bodyInterface = nullptr;
    MyBodyActivationListener body_activation_listener;
    MyContactListener contact_listener;

    JPH::Body* floor;
    JPH::BodyID sphere_id;
};
