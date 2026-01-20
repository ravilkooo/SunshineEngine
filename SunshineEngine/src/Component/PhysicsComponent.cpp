#include <Component/PhysicsComponent.h>
#include <Component/TransformComponent.h>
#include <Component/RenderComponent.h>

#include <Physics/PhysicsSystem.h>

#include <Graphics/Renderer/DeferredRenderer.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>
#include <SimpleMath.h>

PhysicsComponent::PhysicsComponent(SE::UUID objectUUID, TransformComponent* tc)
{
    SetObjecUUID(objectUUID);
    InitTransforms(tc);

    if (tc->m_parentTransform)
    {
        m_motionType = JPH::EMotionType::Kinematic;
    }
}

PhysicsComponent::~PhysicsComponent()
{
	m_physicsSystem->RemoveBody(this);
}

void PhysicsComponent::AddForce(const DXSM::Vector3& inForce)
{
    if (!m_physicsSystem)
        return;

    JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();

    JPH::Vec3 joltForce(
        inForce.x, inForce.y, inForce.z
    );

    bodyInterface.AddForce(m_joltBodyId, joltForce);
}

void PhysicsComponent::AddImpulse(const DXSM::Vector3& inImpulse)
{
    if (!m_physicsSystem)
        return;

    JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();

    JPH::Vec3 joltImpulse(
        inImpulse.x, inImpulse.y, inImpulse.z
    );

    bodyInterface.AddImpulse(m_joltBodyId, joltImpulse);
}

void PhysicsComponent::AddTorque(const DXSM::Vector3& inTorque)
{
    if (!m_physicsSystem)
        return;

    JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();

    JPH::Vec3 joltTorque(
        inTorque.x, inTorque.y, inTorque.z
    );

    bodyInterface.AddTorque(m_joltBodyId, joltTorque);
}

void PhysicsComponent::AddAngularImpulse(const DXSM::Vector3& inAngularImpulse)
{
    if (!m_physicsSystem)
        return;

    JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();

    JPH::Vec3 joltAngularImpulse(
        inAngularImpulse.x, inAngularImpulse.y, inAngularImpulse.z
    );

    bodyInterface.AddAngularImpulse(m_joltBodyId, joltAngularImpulse);
}

DXSM::Vector3 PhysicsComponent::GetAccumulatedForce()
{
    if (m_joltBody)
    {
        JPH::Vec3 f = m_joltBody->GetAccumulatedForce();
        return DXSM::Vector3(f.GetX(), f.GetY(), f.GetZ());
    }
    return DXSM::Vector3::Zero;
}

DXSM::Vector3 PhysicsComponent::GetAccumulatedTorque()
{
    if (m_joltBody)
    {
        JPH::Vec3 t = m_joltBody->GetAccumulatedTorque();
        return DXSM::Vector3(t.GetX(), t.GetY(), t.GetZ());
    }
    return DXSM::Vector3::Zero;
}

DXSM::Vector3 PhysicsComponent::GetAngularVelocity()
{
    if (m_physicsSystem)
    {
        JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();
        if (m_joltBody)
        {
            JPH::Vec3 v = m_joltBody->GetAngularVelocity();
            return DXSM::Vector3(v.GetX(), v.GetY(), v.GetZ());
        }
    }
    return DXSM::Vector3::Zero;
}

DXSM::Vector3 PhysicsComponent::GetLinearVelocity()
{
    if (m_physicsSystem)
    {
        JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();
        if (m_joltBody)
        {
            JPH::Vec3 v = m_joltBody->GetLinearVelocity();
            return DXSM::Vector3(v.GetX(), v.GetY(), v.GetZ());
        }
    }
    return DXSM::Vector3::Zero;
}

DXSM::Vector3 PhysicsComponent::GetPointVelocity(const DXSM::Vector3& inPoint)
{
    if (!m_physicsSystem)
        return DXSM::Vector3::Zero;

    JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();
    JPH::RVec3 p(inPoint.x, inPoint.y, inPoint.z);
    if (bodyInterface.IsAdded(m_joltBodyId))
    {
        JPH::Vec3 v = bodyInterface.GetPointVelocity(m_joltBodyId, p);
        return DXSM::Vector3(v.GetX(), v.GetY(), v.GetZ());
    }
    return DXSM::Vector3::Zero;
}

DXSM::Vector3 PhysicsComponent::GetPosition()
{
    if (!m_physicsSystem)
        return DXSM::Vector3::Zero;

    JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();
    if (bodyInterface.IsAdded(m_joltBodyId))
    {
        JPH::RVec3 pos = bodyInterface.GetPosition(m_joltBodyId);
        return DXSM::Vector3(pos.GetX(), pos.GetY(), pos.GetZ());
    }
    return DXSM::Vector3::Zero;
}

DXSM::Quaternion PhysicsComponent::GetRotation()
{
    if (!m_physicsSystem)
        return DXSM::Quaternion::Identity;

    JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();
    if (bodyInterface.IsAdded(m_joltBodyId))
    {
        JPH::Quat q = bodyInterface.GetRotation(m_joltBodyId);
        return DXSM::Quaternion(q.GetX(), q.GetY(), q.GetZ(), q.GetW());
    }
    return DXSM::Quaternion::Identity;
}

void PhysicsComponent::ResetForce()
{
    if (m_joltBody)
        m_joltBody->ResetForce();
}

void PhysicsComponent::ResetTorque()
{
    if (m_joltBody)
        m_joltBody->ResetTorque();
}

void PhysicsComponent::SetAngularVelocity(const DXSM::Vector3& inAngularVelocity)
{
    if (!m_physicsSystem)
        return;

    JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();
    JPH::Vec3 v(inAngularVelocity.x, inAngularVelocity.y, inAngularVelocity.z);
    bodyInterface.SetAngularVelocity(m_joltBodyId, v);
}

void PhysicsComponent::SetLinearVelocity(const DXSM::Vector3& inLinearVelocity)
{
    if (!m_physicsSystem)
        return;

    JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();
    JPH::Vec3 v(inLinearVelocity.x, inLinearVelocity.y, inLinearVelocity.z);
    bodyInterface.SetLinearVelocity(m_joltBodyId, v);
}

void PhysicsComponent::SetActive(bool active)
{
    if (!m_physicsSystem)
        return;

    JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();
    
    if (active)
    {
        // Add body back to simulation if not already added
        if (!bodyInterface.IsAdded(m_joltBodyId))
        {
            bodyInterface.AddBody(m_joltBodyId, JPH::EActivation::Activate);
        }
        else
        {
            bodyInterface.ActivateBody(m_joltBodyId);
        }
    }
    else
    {
        // Remove from simulation but keep the body data
        if (bodyInterface.IsAdded(m_joltBodyId))
        {
            bodyInterface.RemoveBody(m_joltBodyId);
        }
    }
}

bool PhysicsComponent::IsActive() const
{
    if (!m_physicsSystem)
        return false;

    JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();
    return bodyInterface.IsAdded(m_joltBodyId);
}

void PhysicsComponent::SetObjecUUID(SE::UUID objectUUID) {
    m_objectUUID = objectUUID;
}

// Can have only two values: MOVING or NON_MOVING
void PhysicsComponent::SetObjectLayer(JPH::ObjectLayer layer) { m_objectLayer = layer; }

void PhysicsComponent::SetPosition(const JPH::RVec3& pos) { m_position = pos; }

void PhysicsComponent::SetOrientation(const JPH::Quat& rot) { m_orientation = rot; }

void PhysicsComponent::SetMotionType(JPH::EMotionType type) { m_motionType = type; }

void PhysicsComponent::SetActivation(JPH::EActivation activation) { activation = activation; }

void PhysicsComponent::SetShape(JPH::ShapeRefC shapePtr) { m_shape = shapePtr; }

void PhysicsComponent::InitTransforms(TransformComponent* tc)
{
    transformComp = tc;

    auto wMat = tc->GetWorldMatrix_noLocal();

    DX::XMVECTOR scale, rotation, translation;
    DX::XMMatrixDecompose(&scale, &rotation, &translation, DX::XMLoadFloat4x4(&wMat));

	DXSM::Vector3 _pos;
	DXSM::Quaternion _quat;

    DX::XMStoreFloat3(&_pos, translation);
    DX::XMStoreFloat4(&_quat, rotation);

    m_position.Set(_pos.x, _pos.y, _pos.z);
    m_orientation.Set(_quat.x, _quat.y, _quat.z, _quat.w);
}

JPH::Body* PhysicsComponent::GetBody() const { return m_joltBody; }
JPH::BodyID PhysicsComponent::GetBodyID() const { return m_joltBodyId; }


PhysicsComponent_Info::PhysicsComponent_Info(
    RenderComponent_Info* rc_info,
    TransformComponent_Info* tc_info)
    : m_rc_info(rc_info)
{
    m_colliderData = eastl::make_shared<SE::ColliderData>(SE::ColliderShapeType::Box);

    // Init collider
    auto device = rc_info->m_assignedComponent.get()->GetDevice();
    auto colliderTech = eastl::make_unique<SE_G::ColliderTechnique>(
        device, tc_info->m_assignedComponent.get(), eastl::string("ColliderPass"),
        m_colliderData);

    rc_info->AddTechnique(eastl::move(colliderTech));

    SetCollisionLayer("MOVING");
    if (tc_info->m_assignedComponent->m_parentTransform)
    {
        SetMotion(SE::PhysicsMotionType::Kinematic);
    }
    else
    {
        SetMotion(SE::PhysicsMotionType::Dynamic);
    }
    SetActivation(SE::PhysicsActivation::Activate);
    SetShape(SE::ColliderShapeType::Box);
    SE::ColliderSettings collSettings{};
    collSettings.data.asBox = { { 1.0f, 1.0f, 1.0f } };
    m_colliderData->SetColliderSettings(collSettings);

    m_isValid = true;
}

PhysicsComponent_Info::~PhysicsComponent_Info() {
    if (m_isValid)
        m_rc_info->RemoveTechnique("ColliderPass");
}


// Accessors for physics configuration
SE::ColliderShapeType PhysicsComponent_Info::GetShape() const { return m_colliderData->m_shapeType; }
void PhysicsComponent_Info::SetShape(SE::ColliderShapeType shape)
{
    m_colliderData->SetShapeType(shape);
}

SE::PhysicsMotionType PhysicsComponent_Info::GetMotion() const { return m_motion; }
void PhysicsComponent_Info::SetMotion(SE::PhysicsMotionType motion) { m_motion = motion; }

SE::PhysicsActivation PhysicsComponent_Info::GetActivation() const { return m_activation; }
void PhysicsComponent_Info::SetActivation(SE::PhysicsActivation activation) { m_activation = activation; }

const SE::CollisionLayer& PhysicsComponent_Info::GetCollisionLayer() const { return m_collisionLayer; }
void PhysicsComponent_Info::SetCollisionLayer(const SE::CollisionLayer& layer) { m_collisionLayer = layer; }
void PhysicsComponent_Info::SetCollisionLayer(SE::CollisionLayer&& layer) { m_collisionLayer = eastl::move(layer); }

#define PC_ADD_METHOD(k, fn) k, fn

LUA_REGISTER_COMPONENT(
    PhysicsComponent,
    "PhysicsComponent",
    /* no fields */,
    PHYSICSCOMPONENT_LUA_METHODS_APPLY(PC_ADD_METHOD),
    "getPhysics")
#undef PC_ADD_METHOD
