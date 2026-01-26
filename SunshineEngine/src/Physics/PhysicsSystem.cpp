#include <Physics/PhysicsSystem.h>
#include <Scene.h>
#include <Component/TransformComponent.h>
#include <Component/PhysicsComponent.h>
#include <Component/TriggerComponent.h>

#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

PhysicsSystem::PhysicsSystem() :
    m_bodyEntries(eastl::vector<PhysicsBodyEntry>()),
    m_triggerContactListener()
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
    // m_physicsSystem->SetContactListener(&m_contactListener);
    m_physicsSystem->SetGravity(JPH::Vec3(0, -9.81f, 0));
    m_bodyInterface = &m_physicsSystem->GetBodyInterface();

    m_physicsSystem->SetContactListener(&m_triggerContactListener);
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

class PerceptionBroadPhaseLayerFilter : public JPH::BroadPhaseLayerFilter
{
public:
    PerceptionBroadPhaseLayerFilter()
    {
    }

    virtual bool ShouldCollide(JPH::BroadPhaseLayer inLayer) const override
    {
        // Ignore 'transparent' triggers
        return inLayer != SE::BroadPhaseLayers::TRIGGER;
    }
};

bool PhysicsSystem::PerceptionTrace(const JPH::RVec3& begin,
    const JPH::Vec3& dir,
    float length,
    const eastl::vector<SE::UUID>& ignore,
    SE::UUID* out_id)
{
    if (!m_physicsSystem)
        return false;

    // Build ignore set
    eastl::unordered_set<SE::UUID> ignore_set(ignore.begin(), ignore.end());

    JPH::RRayCast ray(begin, JPH::RVec3(dir * length));
    JPH::RayCastResult   result;
    
    // Cast againts all layers
    EmptyFilter layer_filter = EmptyFilter();
    IgnoreUUIDBodyFilter body_filter(ignore_set, m_physicsSystem->GetBodyLockInterface());
    
    // Create filter for specific broad-phase layer
    PerceptionBroadPhaseLayerFilter bpFilter;

    auto& nq = m_physicsSystem->GetNarrowPhaseQuery(); // locking version for thread safety

    bool hit = nq.CastRay(ray,
        result,
        bpFilter,
        layer_filter,
        body_filter);

    if (!hit)
    {
        return false;
    }

    if (out_id)
    {
        // Retrieve SE::UUID from hit body
        JPH::BodyID body_id = result.mBodyID;

        JPH::BodyLockRead lock(m_physicsSystem->GetBodyLockInterface(), body_id);
        if (!lock.Succeeded())
        {
            *out_id = SE::UUID(0u);
            return false;
        }

        const JPH::Body& body = lock.GetBody();
        *out_id = SE::UUID(body.GetUserData());

        auto n = body.GetWorldSpaceSurfaceNormal(result.mSubShapeID2, ray.GetPointOnRay(result.mFraction));
        // printf("\t\tHit normal: %.2f, %.2f, %.2f\n", n.GetX(), n.GetY(), n.GetZ());

        return true;
    }
	return true;
}
//////////////////////////////////////////
//////////////////////////////////////////

void PhysicsSystem::CreateAndAddBody(PhysicsComponent* physComp) {
    physComp->InitTransforms();

    JPH::ShapeSettings::ShapeResult shapeResult = physComp->m_colliderData.CreateShape();

    if (shapeResult.IsValid()) {
        physComp->m_shape = shapeResult.Get();
    }
    
    if (!physComp->m_shape) {
        // handle error: shape not set
        return;
    }

    JPH::BodyInterface& bodyInterface = Bodies();

    JPH::BodyCreationSettings settings(physComp->m_shape, physComp->m_position, physComp->m_orientation, physComp->m_motionType, physComp->m_objectLayer);
    settings.mObjectLayer = physComp->m_objectLayer;
    settings.mAllowSleeping = (physComp->m_activation != JPH::EActivation::DontActivate);
    settings.mFriction = physComp->m_friction;
    settings.mLinearDamping = physComp->m_linearDamping;
    settings.mAngularDamping = physComp->m_angularDamping;
    settings.mRestitution = physComp->m_restitution;

    physComp->m_joltBody = bodyInterface.CreateBody(settings);
    physComp->m_joltBodyId = physComp->m_joltBody->GetID();
    physComp->m_joltBody->SetUserData(physComp->m_objectUUID.m_UUID);
	physComp->m_physicsSystem = this;

    m_bodyEntries.push_back({ physComp->m_joltBodyId });
    m_bodyInterface->AddBody(physComp->m_joltBodyId, physComp->m_activation);
}

void PhysicsSystem::RemoveBody(PhysicsComponent* physComp)
{
    if (!physComp || !m_bodyInterface)
        return;

    JPH::BodyID bodyId = physComp->m_joltBodyId;

    // Remove from body entries
    auto it = eastl::find_if(m_bodyEntries.begin(), m_bodyEntries.end(),
        [bodyId](const PhysicsBodyEntry& entry) {
            return entry.m_joltBodyId == bodyId;
        });

    if (it != m_bodyEntries.end())
    {
        m_bodyEntries.erase(it);
    }

    // Remove from physics world
    if (m_bodyInterface->IsAdded(bodyId))
    {
        m_bodyInterface->RemoveBody(bodyId);
    }

    // Destroy the body
    m_bodyInterface->DestroyBody(bodyId);

    // Clear component references
    physComp->m_joltBody = nullptr;
    physComp->m_joltBodyId = JPH::BodyID();
    physComp->m_physicsSystem = nullptr;
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

        SE::UUID objectUUID = SE::UUID((std::uint64_t)m_bodyInterface->GetUserData(bodyEntry.m_joltBodyId));
        auto objPtr = scene->GetGameObjectByUUID(objectUUID);
        if (!objPtr)
			continue;

        if (m_bodyInterface->GetMotionType(bodyEntry.m_joltBodyId) == JPH::EMotionType::Dynamic)
        {
            //JPH::RMat44 bodyTransform = m_bodyInterface->GetWorldTransform(bodyEntry.m_joltBodyId);

            JPH::RVec3 position = m_bodyInterface->GetCenterOfMassPosition(bodyEntry.m_joltBodyId);
            JPH::Quat quatRot = m_bodyInterface->GetRotation(bodyEntry.m_joltBodyId);


            auto tc = objPtr->GetComponent<TransformComponent>();

            tc->m_position =
                DXSM::Vector3(position.mF32
                );
            tc->m_rotation =
                DXSM::Vector3(DXSM::Quaternion(quatRot.mValue.mF32).ToEuler()
                );
        }
        else if (m_bodyInterface->GetMotionType(bodyEntry.m_joltBodyId) == JPH::EMotionType::Kinematic)
        {
            SE::UUID objectUUID = SE::UUID((std::uint64_t)m_bodyInterface->GetUserData(bodyEntry.m_joltBodyId));

            auto gameObject = scene->GetGameObjectByUUID(objectUUID);
            if (!gameObject)
                continue;

            auto tc = gameObject->GetComponent<TransformComponent>();
            if (!tc)
                continue;
            /*
            auto wMat = tc->GetWorldMatrix_noLocal();

            DX::XMVECTOR scale, rotation, translation;
            DX::XMMatrixDecompose(&scale, &rotation, &translation, DX::XMLoadFloat4x4(&wMat));
            */

            DXSM::Vector3 _pos = tc->GetAbsoluteWorldPosition();
            DXSM::Quaternion _quat = tc->GetAbsoluteWorldRotation_quat();

            // Push TransformComponent data into the kinematic body
            const JPH::RVec3 targetPos(_pos.x, _pos.y, _pos.z);
            const JPH::Quat targetRot(_quat.x, _quat.y, _quat.z, _quat.w);

            m_bodyInterface->SetPositionAndRotation(
                bodyEntry.m_joltBodyId,
                targetPos,
                targetRot,
                JPH::EActivation::Activate);
        }
    }

    for (auto triggerJoltId : m_activeTriggers) {

        SE::UUID objectUUID = SE::UUID((std::uint64_t)m_bodyInterface->GetUserData(triggerJoltId));
        auto objPtr = scene->GetGameObjectByUUID(objectUUID);
        if (!objPtr)
            continue;

        if (m_bodyInterface->GetMotionType(triggerJoltId) == JPH::EMotionType::Dynamic)
        {
            //JPH::RMat44 bodyTransform = m_bodyInterface->GetWorldTransform(triggerJoltId);

            JPH::RVec3 position = m_bodyInterface->GetCenterOfMassPosition(triggerJoltId);
            JPH::Quat quatRot = m_bodyInterface->GetRotation(triggerJoltId);


            auto tc = objPtr->GetComponent<TransformComponent>();

            tc->m_position =
                DXSM::Vector3(position.mF32
                );
            tc->m_rotation =
                DXSM::Vector3(DXSM::Quaternion(quatRot.mValue.mF32).ToEuler()
                );
        }
        else if (m_bodyInterface->GetMotionType(triggerJoltId) == JPH::EMotionType::Kinematic)
        {
            SE::UUID objectUUID = SE::UUID((std::uint64_t)m_bodyInterface->GetUserData(triggerJoltId));

            auto gameObject = scene->GetGameObjectByUUID(objectUUID);
            if (!gameObject)
                continue;

            auto tc = gameObject->GetComponent<TransformComponent>();
            if (!tc)
                continue;
            /*
            auto wMat = tc->GetWorldMatrix_noLocal();

            DX::XMVECTOR scale, rotation, translation;
            DX::XMMatrixDecompose(&scale, &rotation, &translation, DX::XMLoadFloat4x4(&wMat));
            */

            DXSM::Vector3 _pos = tc->GetAbsoluteWorldPosition();
            DXSM::Quaternion _quat = tc->GetAbsoluteWorldRotation_quat();

            // Push TransformComponent data into the kinematic body
            const JPH::RVec3 targetPos(_pos.x, _pos.y, _pos.z);
            const JPH::Quat targetRot(_quat.x, _quat.y, _quat.z, _quat.w);

            m_bodyInterface->SetPositionAndRotation(
                triggerJoltId,
                targetPos,
                targetRot,
                JPH::EActivation::Activate);
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
    // FlushCommands();

    m_physicsSystem->Update(
        dt, /*collisionSteps*/1,
        // /*integrationSubSteps*/ 1,
        m_tempAllocator.get(), m_jobSystem.get());

    UpdateTriggerOverlaps();

    // FlushCommands();
}

void PhysicsSystem::ClearAllBodies()
{
    for (auto body : m_bodyEntries)
    {
        m_bodyInterface->RemoveBody(body.m_joltBodyId);
        m_bodyInterface->DestroyBody(body.m_joltBodyId);
    }
    m_bodyEntries.clear();

    // eastl::vector<JPH::BodyID> m_activeTriggers;
    for (auto body : m_activeTriggers)
    {
        m_bodyInterface->RemoveBody(body);
        m_bodyInterface->DestroyBody(body);
    }
    m_activeTriggers.clear();

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

void PhysicsSystem::UpdateTriggerOverlaps()
{
    eastl::vector<TriggerExitEvent> exitEvents;
    m_triggerContactListener.FetchExitEvents(exitEvents);

    for (const TriggerExitEvent& e : exitEvents)
    {
        auto triggerGO = Scene::GetInstance().GetGameObjectByUUID(e.Trigger);
        if (!triggerGO)
            continue;

        auto triggerComp = triggerGO->GetComponent<TriggerComponent>();
        if (!triggerComp)
            continue;

        triggerComp->OnExit(e.Other);
    }
}

void PhysicsSystem::CreateAndAddTrigger(TriggerComponent* triggerComp) {
    triggerComp->InitTransforms();

    JPH::ShapeSettings::ShapeResult shapeResult = triggerComp->m_colliderData.CreateShape();

    if (shapeResult.IsValid()) {
        triggerComp->m_shape = shapeResult.Get();
    }

    if (!triggerComp->m_shape) {
        // handle error: shape not set
        return;
    }

    JPH::BodyInterface& bodyInterface = Bodies();

    JPH::BodyCreationSettings settings(triggerComp->m_shape,
        triggerComp->m_position, triggerComp->m_orientation,
        triggerComp->s_triggerMotionType, triggerComp->s_triggerObjectLayer);
    settings.mObjectLayer = triggerComp->s_triggerObjectLayer;
    settings.mAllowSleeping = (triggerComp->s_triggerActivation != JPH::EActivation::DontActivate);
    settings.mIsSensor = true;

    triggerComp->m_joltBody = bodyInterface.CreateBody(settings);
    triggerComp->m_joltBodyId = triggerComp->m_joltBody->GetID();
    triggerComp->m_joltBody->SetUserData(triggerComp->m_objectUUID.m_UUID);
    triggerComp->m_physicsSystem = this;

    //m_bodyEntries.push_back({ triggerComp->m_joltBodyId });
    m_bodyInterface->AddBody(triggerComp->m_joltBodyId, triggerComp->s_triggerActivation);

    m_activeTriggers.push_back(triggerComp->m_joltBodyId);
}

void PhysicsSystem::RemoveTrigger(TriggerComponent* triggerComp)
{
    if (!triggerComp || !m_bodyInterface)
        return;

    JPH::BodyID bodyId = triggerComp->m_joltBodyId;

    // Remove from body entries
    auto it = eastl::find_if(m_activeTriggers.begin(), m_activeTriggers.end(),
        [bodyId](const PhysicsBodyEntry& entry) {
            return entry.m_joltBodyId == bodyId;
        });
    if (it != m_activeTriggers.end())
    {
        m_activeTriggers.erase(it);
    }

    // Remove from physics world
    if (m_bodyInterface->IsAdded(bodyId))
    {
        m_bodyInterface->RemoveBody(bodyId);
    }

    // Destroy the body
    m_bodyInterface->DestroyBody(bodyId);

    // Clear component references
    triggerComp->m_joltBody = nullptr;
    triggerComp->m_joltBodyId = JPH::BodyID();
    triggerComp->m_physicsSystem = nullptr;
}


void PhysicsSystem::SetGravity(DXSM::Vector3 inGravity)
{
    EnqueueCommand([this, inGravity]()
        {
            m_physicsSystem->SetGravity(JPH::Vec3(inGravity.x, inGravity.y, inGravity.z));
        });
}

DXSM::Vector3 PhysicsSystem::GetGravity()
{
    auto currGrav = m_physicsSystem->GetGravity();
    return DXSM::Vector3(currGrav.GetX(), currGrav.GetY(), currGrav.GetZ());
}

void PhysicsSystem::EnqueueCommand(std::function<void()> fn)
{
    std::lock_guard<std::mutex> l(m_cmdMutex);
    m_cmds.push_back(std::move(fn));
}

void PhysicsSystem::FlushCommands()
{
    std::vector<std::function<void()>> local;
    {
        std::lock_guard<std::mutex> l(m_cmdMutex);
        local.swap(m_cmds);
    }
    for (auto& fn : local)
        fn();
}
