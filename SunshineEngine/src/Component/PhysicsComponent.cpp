#include <Component/PhysicsComponent.h>
#include <Physics/PhysicsSystem.h>
#include <Component/TransformComponent.h>

PhysicsComponent::~PhysicsComponent() {
    JPH::BodyInterface& bodyInterface = m_physicsSystem->Bodies();
    bodyInterface.RemoveBody(m_joltBodyId);
    bodyInterface.DestroyBody(m_joltBodyId);
}

void PhysicsComponent::SetObjecUUID(SE::UUID objectUUID) {
    m_objectUUID = objectUUID;
}

void PhysicsComponent::SetObjectLayer(JPH::ObjectLayer layer) { m_objectLayer = layer; }

void PhysicsComponent::SetPosition(const JPH::RVec3& pos) { m_position = pos; }

void PhysicsComponent::SetOrientation(const JPH::Quat& rot) { m_orientation = rot; }

void PhysicsComponent::SetMotionType(JPH::EMotionType type) { m_motionType = type; }

void PhysicsComponent::SetActivation(JPH::EActivation activation) { activation = activation; }

void PhysicsComponent::SetShape(JPH::ShapeRefC shapePtr) { m_shape = shapePtr; }

// Create and add body to physics system
void PhysicsComponent::CreateBody(eastl::shared_ptr<PhysicsSystem> physicsSystem)
{
    m_physicsSystem = physicsSystem;
    JPH::BodyInterface& bodyInterface = physicsSystem->Bodies();

    if (!m_shape) {
        // handle error: shape not set
        return;
    }
    JPH::BodyCreationSettings settings(m_shape, m_position, m_orientation, m_motionType, m_objectLayer);
    settings.mObjectLayer = m_objectLayer;
    settings.mAllowSleeping = (m_activation != JPH::EActivation::DontActivate);

    m_joltBody = bodyInterface.CreateBody(settings);
    m_joltBodyId = m_joltBody->GetID();
    physicsSystem->AddBody(m_joltBodyId, m_objectUUID, m_activation);

    /*
    joltBodyId = bodyInterface.CreateAndAddBody(settings, activation);
    joltBody = bodyInterface.GetBody(joltBodyId);
    */
}

void PhysicsComponent::InitTransforms(TransformComponent* tc) {
    m_position.Set(tc->m_position.x, tc->m_position.y, tc->m_position.z);

    auto quat = DXSM::Quaternion::CreateFromYawPitchRoll(tc->m_rotation.y, tc->m_rotation.x, tc->m_rotation.z);

    m_orientation.Set(quat.x, quat.y, quat.z, quat.w);
}

JPH::Body* PhysicsComponent::GetBody() const { return m_joltBody; }
JPH::BodyID PhysicsComponent::GetBodyID() const { return m_joltBodyId; }