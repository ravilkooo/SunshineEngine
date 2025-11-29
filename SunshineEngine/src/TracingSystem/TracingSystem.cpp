#include <TracingSystem/TracingSystem.h>

#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h>
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>

#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

#include <Component/Component.h>
#include <Component/TransformComponent.h>
#include <Scene.h>

// Minimal trace function used when TracingSystem manages its own Jolt runtime
static void TracingSystem_Trace(const char* inFMT, ...)
{
    // swallow or forward to std::cout if helpful
    va_list args;
    va_start(args, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, args);
    va_end(args);
    // optionally print: std::cout << buffer << std::endl;
}

bool TracingSystem::Trace(const JPH::RVec3& begin,
    const JPH::Vec3& dir,
    float length,
    JPH::ObjectLayer layer,
    const eastl::vector<SE::UUID>& ignore,
    SE::UUID* out_id)
{
    if (!m_physics)
        return false;

    // Build ignore set
    eastl::unordered_set<SE::UUID> ignore_set(ignore.begin(), ignore.end());

    JPH::RRayCast ray(begin, begin + JPH::RVec3(dir * length));

    JPH::RayCastResult   result;

    SingleLayerFilter layer_filter(layer);
    IgnoreUUIDBodyFilter body_filter(ignore_set, m_physics->GetBodyLockInterface());

    auto& nq = m_physics->GetNarrowPhaseQuery(); // locking version for thread safety

    bool hit = nq.CastRay(ray,
        result,
        JPH::BroadPhaseLayerFilter(), // accept all BP layers or plug your own
        layer_filter,
        body_filter);

    if (!hit || !out_id)
        return hit;

    // Retrieve SE::UUID from hit body
    JPH::BodyID body_id = result.mBodyID;

    JPH::BodyLockRead lock(m_physics->GetBodyLockInterface(), body_id);
    if (!lock.Succeeded())
        return false;

    const JPH::Body& body = lock.GetBody();
    *out_id = *reinterpret_cast<const SE::UUID*>(body.GetUserData());
    return true;
}

TracingSystem::TracingSystem()
{
    // create our own Jolt runtime
    JPH::RegisterDefaultAllocator();
    JPH::Trace = TracingSystem_Trace;

    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    // allocate temp/job systems
    m_tempAllocator = eastl::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
    m_jobSystem = eastl::make_unique<JPH::JobSystemThreadPool>(
        JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers,
        std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 1
    );

    // create tables and filters needed by PhysicsSystem::Init
    // use simple table sizes - adjust if application has more layers
    unsigned int numObjectLayers = 2;
    unsigned int numBroadPhaseLayers = 2;
    m_bpInterface = eastl::make_unique<JPH::BroadPhaseLayerInterfaceTable>(numObjectLayers, numBroadPhaseLayers);
    m_objectPairFilter = eastl::make_unique<JPH::ObjectLayerPairFilterTable>(numObjectLayers);
    // ObjectVsBroadPhaseLayerFilterTable requires references for its constructor parameters
    m_objectVsBpFilter = eastl::make_unique<JPH::ObjectVsBroadPhaseLayerFilterTable>(
        *m_bpInterface, numBroadPhaseLayers,
        *m_objectPairFilter, numObjectLayers);

    m_physicsSystem = eastl::make_unique<JPH::PhysicsSystem>();
    const UINT maxBodies = 8192, numBodyMutexes = 0, maxBodyPairs = 8192, maxContactConstraints = 8192;
    m_physicsSystem->Init(
        maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints,
        *m_bpInterface, *m_objectVsBpFilter, *m_objectPairFilter);

    // We own the system
    m_physics = m_physicsSystem.get();
    m_bodyInterface = &m_physicsSystem->GetBodyInterface();
    m_ownsPhysics = true;
}

TracingSystem::~TracingSystem()
{
    if (m_ownsPhysics) {
        ClearAllBodies();

        // shutdown/cleanup: unregister types & destroy factory if we created it
        JPH::UnregisterTypes();

        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;

        // reset unique_ptrs in reverse order
        m_physicsSystem.reset();
        m_objectPairFilter.reset();
        m_objectVsBpFilter.reset();
        m_bpInterface.reset();
        m_jobSystem.reset();
        m_tempAllocator.reset();

        m_physics = nullptr;
        m_ownsPhysics = false;
    }
}

void TracingSystem::CreateAndAddBody(TracedBody* tracedBody)
{
    JPH::BodyInterface& bodyInterface = Bodies();

    if (!tracedBody->m_shape) {
        // handle error: shape not set
        return;
    }
    JPH::BodyCreationSettings settings(
        tracedBody->m_shape, tracedBody->m_position,
        tracedBody->m_orientation, tracedBody->m_motionType,
        tracedBody->m_objectLayer);

    settings.mObjectLayer = tracedBody->m_objectLayer;
    settings.mAllowSleeping = (tracedBody->m_activation != JPH::EActivation::DontActivate);

    tracedBody->m_joltBody = bodyInterface.CreateBody(settings);
    tracedBody->m_joltBodyId = tracedBody->m_joltBody->GetID();
    tracedBody->m_joltBody->SetUserData(tracedBody->m_objectUUID.m_UUID);

    m_bodyEntries.push_back( tracedBody->m_joltBodyId );
    m_bodyInterface->AddBody(tracedBody->m_joltBodyId, tracedBody->m_activation);
}

// Add objects before this step
void TracingSystem::FinalizeScene()
{
    m_physicsSystem->OptimizeBroadPhase();
}

void TracingSystem::SyncronizeTransforms(Scene* scene)
{
    for (auto joltBodyId : m_bodyEntries) {

        //JPH::RMat44 bodyTransform = m_bodyInterface->GetWorldTransform(joltBodyId);

        JPH::RVec3 position = m_bodyInterface->GetCenterOfMassPosition(joltBodyId);
        JPH::Quat quatRot = m_bodyInterface->GetRotation(joltBodyId);

        SE::UUID objectUUID = SE::UUID((std::uint64_t)m_bodyInterface->GetUserData(joltBodyId));

        auto go = scene->GetGameObjectByUUID(objectUUID);
        if (!go)
            continue;

        auto tr = go->GetComponent<TransformComponent>();
        if (!tr)
            continue;

        tr->m_position =
            DXSM::Vector3(position.mF32
            );
        tr->m_rotation =
            DXSM::Vector3(DXSM::Quaternion(quatRot.mValue.mF32).ToEuler()
            );
    }
}

void TracingSystem::ClearAllBodies()
{
    for (auto joltBodyId : m_bodyEntries)
    {
        m_bodyInterface->RemoveBody(joltBodyId);
        m_bodyInterface->DestroyBody(joltBodyId);
    }
    m_bodyEntries.clear();
}
