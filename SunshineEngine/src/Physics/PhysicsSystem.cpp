#include <Physics/PhysicsSystem.h>
#include <Scene.h>
#include <Component/TransformComponent.h>

#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

PhysicsSystem::PhysicsSystem() :
    m_bodyEntries(eastl::vector<PhysicsBodyEntry>())
{
    JPH::RegisterDefaultAllocator();
    //JPH::Trace = [](const char* fmt, ...) {}; // hook your logger
    JPH::Trace = TraceImpl;

    JPH::Factory::sInstance = new JPH::Factory();

    JPH::RegisterTypes();

    //tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
#ifdef JPH_DISABLE_TEMP_ALLOCATOR
    tempAllocator = new TempAllocatorMalloc();
#else
    m_tempAllocator = eastl::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
#endif

    m_jobSystem = eastl::make_unique<JPH::JobSystemThreadPool>(
        JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1
    );

    // bpInterface = eastl::make_unique<JPH::BroadPhaseLayerInterfaceTable>(
    //     2, 2);
    // objectVsBpFilter = eastl::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
    // objectPairFilter = eastl::make_unique<JPH::ObjectLayerPairFilterTable>(2);

    m_bpInterface = eastl::make_unique<BPLayerInterfaceImpl>();
    m_objectVsBpFilter = eastl::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
    m_objectPairFilter = eastl::make_unique<ObjectLayerPairFilterImpl>();

    m_physicsSystem = eastl::make_unique<JPH::PhysicsSystem>();
    const UINT maxBodies = 8192, numBodyMutexes = 0, maxBodyPairs = 8192, maxContactConstraints = 8192;
    m_physicsSystem->Init(
        maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints,
        *m_bpInterface, *m_objectVsBpFilter, *m_objectPairFilter);
    m_physicsSystem->SetBodyActivationListener(&m_bodyActivationListener);
    m_physicsSystem->SetContactListener(&m_contactListener);
    m_physicsSystem->SetGravity(JPH::Vec3(0, -9.81f, 0));
    m_bodyInterface = &m_physicsSystem->GetBodyInterface();
}

PhysicsSystem::~PhysicsSystem()
{
    ClearAllBodies();

    // Unregisters all types with the factory and cleans up the default material
    JPH::UnregisterTypes();

    // Destroy the factory
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;

    m_isValid = false;

    m_bodyInterface = nullptr;
}


//////////////////////////////////////////
// FOR TRACING ONLY (GAI)
//////////////////////////////////////////

bool PhysicsSystem::Trace(const JPH::RVec3& begin,
    const JPH::Vec3& dir,
    float length,
    JPH::ObjectLayer layer,
    const eastl::vector<SE::UUID>& ignore,
    SE::UUID* out_id)
{
    if (!m_physicsSystem)
        return false;

    // Build ignore set
    eastl::unordered_set<SE::UUID> ignore_set(ignore.begin(), ignore.end());

    JPH::RRayCast ray(begin, begin + JPH::RVec3(dir * length));

    JPH::RayCastResult   result;
    
    // Cast againts all layers
    EmptyFilter layer_filter = EmptyFilter();
    IgnoreUUIDBodyFilter body_filter(ignore_set, m_physicsSystem->GetBodyLockInterface());

    auto& nq = m_physicsSystem->GetNarrowPhaseQuery(); // locking version for thread safety

    bool hit = nq.CastRay(ray,
        result,
        JPH::BroadPhaseLayerFilter(), // accept all BP layers or plug your own
        layer_filter,
        body_filter);

    if (!hit || !out_id)
        return hit;

    // Retrieve SE::UUID from hit body
    JPH::BodyID body_id = result.mBodyID;

    JPH::BodyLockRead lock(m_physicsSystem->GetBodyLockInterface(), body_id);
    if (!lock.Succeeded())
        return false;

    const JPH::Body& body = lock.GetBody();
    *out_id = *reinterpret_cast<const SE::UUID*>(body.GetUserData());
    return true;
}
//////////////////////////////////////////
//////////////////////////////////////////

void PhysicsSystem::CreateAndAddBody(PhysicsComponent* physComp) {

    JPH::BodyInterface& bodyInterface = Bodies();

    if (!physComp->m_shape) {
        // handle error: shape not set
        return;
    }
    JPH::BodyCreationSettings settings(physComp->m_shape, physComp->m_position, physComp->m_orientation, physComp->m_motionType, physComp->m_objectLayer);
    settings.mObjectLayer = physComp->m_objectLayer;
    settings.mAllowSleeping = (physComp->m_activation != JPH::EActivation::DontActivate);

    physComp->m_joltBody = bodyInterface.CreateBody(settings);
    physComp->m_joltBodyId = physComp->m_joltBody->GetID();
    physComp->m_joltBody->SetUserData(physComp->m_objectUUID.m_UUID);

    m_bodyEntries.push_back({ physComp->m_joltBodyId });
    m_bodyInterface->AddBody(physComp->m_joltBodyId, physComp->m_activation);
}

// Add objects before this step
void PhysicsSystem::FinalizeScene() {
    /*
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
    bodyInterface->CreateAndAddBody(floor->GetID(), JPH::EActivation::DontActivate);

    // Now create a dynamic body to bounce on the floor
    // Note that this uses the shorthand version of creating and adding a body to the world
    JPH::BodyCreationSettings sphere_settings(new JPH::SphereShape(0.5f), JPH::RVec3(0.0, 2.0, 0.0),
        JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic,
        Layers::MOVING);
    sphere_id = bodyInterface->CreateAndAddBody(sphere_settings, JPH::EActivation::Activate);

    // Now you can interact with the dynamic body, in this case we're going to give it a velocity.
    // (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
    bodyInterface->SetLinearVelocity(sphere_id, JPH::Vec3(0.0f, -5.0f, 0.0f));
    */

    m_physicsSystem->OptimizeBroadPhase();
    m_isValid = true;
}

void PhysicsSystem::SyncronizeTransforms(Scene* scene) {
    for (auto bodyEntry : m_bodyEntries) {

        if (m_bodyInterface->GetMotionType(bodyEntry.m_joltBodyId) == JPH::EMotionType::Dynamic)
        {
            //JPH::RMat44 bodyTransform = m_bodyInterface->GetWorldTransform(bodyEntry.m_joltBodyId);

            JPH::RVec3 position = m_bodyInterface->GetCenterOfMassPosition(bodyEntry.m_joltBodyId);
            JPH::Quat quatRot = m_bodyInterface->GetRotation(bodyEntry.m_joltBodyId);

            SE::UUID objectUUID = SE::UUID((std::uint64_t)m_bodyInterface->GetUserData(bodyEntry.m_joltBodyId));

            auto tc = scene->GetGameObjectByUUID(
                objectUUID)->GetComponent<TransformComponent>();

            tc->m_position =
                DXSM::Vector3(position.mF32
                );
            tc->m_rotation =
                DXSM::Vector3(DXSM::Quaternion(quatRot.mValue.mF32).ToEuler()
                );
        }        
    }
}

void PhysicsSystem::Step(float dt) {
    /*
    // Output current position and velocity of the sphere
    JPH::RVec3 position = bodyInterface->GetCenterOfMassPosition(sphere_id);
    JPH::Vec3 velocity = bodyInterface->GetLinearVelocity(sphere_id);
    //std::cout << " :: Position = (" << position.GetX() << ", " << position.GetY() << ", " << position.GetZ() << "), Velocity = (" << velocity.GetX() << ", " << velocity.GetY() << ", " << velocity.GetZ() << ")" << std::endl;
    */

    m_physicsSystem->Update(
        dt, /*collisionSteps*/1,
        // /*integrationSubSteps*/ 1,
        m_tempAllocator.get(), m_jobSystem.get());
}

void PhysicsSystem::ClearAllBodies()
{
    for (auto body : m_bodyEntries)
    {
        m_bodyInterface->RemoveBody(body.m_joltBodyId);
        m_bodyInterface->DestroyBody(body.m_joltBodyId);
    }
    m_bodyEntries.clear();

    /*
    for (body in physicsScene)
    {
        bodyInterface->RemoveBody(body->GetID());
        bodyInterface->DestroyBody(body->GetID());
    }
    */

    /*
    // Remove the sphere from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
    bodyInterface->RemoveBody(sphere_id);

    // Destroy the sphere. After this the sphere ID is no longer valid.
    bodyInterface->DestroyBody(sphere_id);

    // Remove and destroy the floor
    bodyInterface->RemoveBody(floor->GetID());
    bodyInterface->DestroyBody(floor->GetID());
    */
}

JPH::PhysicsSystem& PhysicsSystem::GetWorld() { return *m_physicsSystem; }

JPH::BodyInterface& PhysicsSystem::Bodies() { return *m_bodyInterface; }

