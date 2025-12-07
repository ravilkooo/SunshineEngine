#include <Component/PhysicsComponent.h>
#include <Component/TransformComponent.h>
#include <Component/RenderComponent.h>

#include <Physics/PhysicsSystem.h>

#include <Graphics/Renderer/DeferredRenderer.h>


PhysicsComponent::PhysicsComponent(SE::UUID objectUUID, TransformComponent* tc)
{
    SetObjecUUID(objectUUID);
    InitTransforms(tc);
}

PhysicsComponent::~PhysicsComponent()
{

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
    
    m_position.Set(tc->m_position.x, tc->m_position.y, tc->m_position.z);
    auto quat = DXSM::Quaternion::CreateFromYawPitchRoll(tc->m_rotation.y, tc->m_rotation.x, tc->m_rotation.z);
    m_orientation.Set(quat.x, quat.y, quat.z, quat.w);
}

JPH::Body* PhysicsComponent::GetBody() const { return m_joltBody; }
JPH::BodyID PhysicsComponent::GetBodyID() const { return m_joltBodyId; }


PhysicsComponent_Info::PhysicsComponent_Info(
    RenderComponent_Info* rc_info,
    TransformComponent_Info* tc_info)
    : m_rc_info(rc_info)
{
    m_colliderData = eastl::make_shared<SE::ColliderData>(SE::ColliderShapeType::Box);
    SE::PhysicsMotionType m_motion = SE::PhysicsMotionType::Static;
    SE::PhysicsActivation m_activation = SE::PhysicsActivation::DontActivate;

    // Init collider
    auto device = rc_info->m_assignedComponent.get()->GetDevice();
    auto colliderTech = eastl::make_unique<SE_G::ColliderTechnique>(
        device, tc_info->m_assignedComponent.get(), eastl::string("ColliderPass"),
        m_colliderData);

    rc_info->AddTechnique(eastl::move(colliderTech));

    SetCollisionLayer("MOVING");
    SetMotion(SE::PhysicsMotionType::Dynamic);
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
