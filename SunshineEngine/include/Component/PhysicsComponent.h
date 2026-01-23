#pragma once
#include <EASTL/unique_ptr.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/EActivation.h>

#include <Graphics/Renderer/Technique/ColliderTechnique.h>

#include <Component/Component.h>
#include <Physics/CollisionUtils.h>
#include <Physics/PhysicsEnums.h>
#include <Utils/UUID.h>


class TransformComponent;

class PhysicsSystem;

class PhysicsComponent :
    public Component
{
    friend class PhysicsComponent_Info;
    friend class PhysicsSystem;
public:

    PhysicsComponent() = default;
    PhysicsComponent(SE::UUID objectUUID, TransformComponent* tc);
    ~PhysicsComponent();

    PhysicsComponent(const PhysicsComponent&) = delete;
    PhysicsComponent& operator=(const PhysicsComponent&) = delete;

    PhysicsComponent(PhysicsComponent&&) noexcept = default;
    PhysicsComponent& operator=(PhysicsComponent&&) noexcept = default;

    const std::type_info& getType() const override {
        return typeid(PhysicsComponent);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::PHYSICS;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    // Apply force to physics object
    void AddForce(const DXSM::Vector3& inForce);

    // Apply impulse to physics object
    void AddImpulse(const DXSM::Vector3& inImpulse);

    // Apply torque to physics object
    void AddTorque(const DXSM::Vector3& inTorque);

    // Apply angular impulse to physics object
    void AddAngularImpulse(const DXSM::Vector3& inAngularImpulse);

    // Query accumulated forces/torques and velocities
    DXSM::Vector3 GetAccumulatedForce();
    DXSM::Vector3 GetAccumulatedTorque();
    DXSM::Vector3 GetAngularVelocity();
    DXSM::Vector3 GetLinearVelocity();
    DXSM::Vector3 GetPointVelocity(const DXSM::Vector3& inPoint);
    DXSM::Vector3 GetPosition();
    DXSM::Quaternion GetRotation();

    // Reset accumulated force/torque
    void ResetForce();
    void ResetTorque();

    // Set velocities
    void SetAngularVelocity(const DXSM::Vector3& inAngularVelocity);
    void SetLinearVelocity(const DXSM::Vector3& inLinearVelocity);

    // Enable/disable physics body (removes from simulation but keeps data)
    void SetActive(bool active);
    bool IsActive() const;
    
    // Setters for configuration before adding body
    void SetObjecUUID(SE::UUID objectUUID);
    void SetObjectLayer(JPH::ObjectLayer layer);
    void SetPosition(const JPH::RVec3& pos);
    void SetOrientation(const JPH::Quat& rot);
    void SetMotionType(JPH::EMotionType type);
    void SetActivation(JPH::EActivation activation);
    void SetShape(JPH::ShapeRefC shapePtr);

    // Friction
    void SetFriction(float inFriction);
    float GetFriction();

    // Restitution
    void SetRestitution(float inRestitution);
    float GetRestitution();

    // GravityFactor
    void SetGravityFactor(float inGravityFactor);
    float GetGravityFactor();

    void InitTransforms(TransformComponent* tc);

    JPH::Body* GetBody() const;
    JPH::BodyID GetBodyID() const;

    void FromJson(const json& j) override;

private:
    PhysicsSystem* m_physicsSystem;

    TransformComponent* transformComp;

    SE::UUID m_objectUUID;
    JPH::Body* m_joltBody;
    JPH::BodyID m_joltBodyId;

    // Configuration properties before creating body
    JPH::RVec3 m_position = JPH::RVec3::sZero();
    JPH::Quat m_orientation = JPH::Quat::sIdentity();
    JPH::EMotionType m_motionType = JPH::EMotionType::Static;
    JPH::EActivation m_activation = JPH::EActivation::Activate;
    JPH::ObjectLayer m_objectLayer = SE::Layers::MOVING; // default layer
    JPH::ShapeRefC m_shape = nullptr;

    // Friction and damping
    float m_friction = 0.2f;
    float m_linearDamping = 0.05f;
    float m_angularDamping = 0.05f;

    // Restitution
    float m_restitution = 0.0f;

    SE::ColliderTransforms m_transformsData;
    /*
    ColliderTransformCB m_transformMat;
    */
};

class RenderComponent_Info;
class TransformComponent_Info;

class PhysicsComponent_Info :
    public Component_Info
{
public:
    static const SE::ComponentType s_componentType = SE::ComponentType::PHYSICS;
    PhysicsComponent_Info() {};
    PhysicsComponent_Info(RenderComponent_Info* rc_info, 
        TransformComponent_Info* tc_info);
    ~PhysicsComponent_Info();

    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    const std::type_info& getType() const override {
        return typeid(PhysicsComponent_Info);
    }

    bool IsAssigned() override { return false; }

    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;

    // Accessors for physics configuration
    SE::ColliderShapeType GetShape() const;
    void SetShape(SE::ColliderShapeType shape);

    SE::PhysicsMotionType GetMotion() const;
    void SetMotion(SE::PhysicsMotionType motion);

    SE::PhysicsActivation GetActivation() const;
    void SetActivation(SE::PhysicsActivation activation);

    const SE::CollisionLayer& GetCollisionLayer() const;
    void SetCollisionLayer(const SE::CollisionLayer& layer);
    void SetCollisionLayer(SE::CollisionLayer&& layer);


    /*
    SE::ColliderTransforms GetTransformData();
    void SetTransformData(SE::ColliderTransforms transform);

    SE::ColliderShapeType GetShapeType();
    void SetShapeType(SE::ColliderShapeType shapeType);

    SE::ColliderSettings GetColliderSettings();
    void SetColliderSettings(SE::ColliderSettings settings);
    */
    
    bool m_isValid = false;
    RenderComponent_Info* m_rc_info;

    // All colliders settings
    eastl::shared_ptr<SE::ColliderData> m_colliderData;

    SE::PhysicsMotionType m_motion = SE::PhysicsMotionType::Static;
    SE::PhysicsActivation m_activation = SE::PhysicsActivation::DontActivate;

    // Friction and damping
    float m_friction = 0.2f;
    float m_linearDamping = 0.05f;
    float m_angularDamping = 0.05f;
    
    // Restitution
    float m_restitution = 0.0f;

    SE::CollisionLayer m_collisionLayer = "MOVING";
};

// Macro listing methods of PhysicsComponent to expose in Lua bindings
#ifndef PHYSICSCOMPONENT_LUA_METHODS_APPLY
#define PHYSICSCOMPONENT_LUA_METHODS_APPLY(FM) \
    FM("addForce", [](PhysicsComponent* self, const DXSM::Vector3& inForce){ self->AddForce(inForce); }), \
    FM("addImpulse", [](PhysicsComponent* self, const DXSM::Vector3& inImpulse){ self->AddImpulse(inImpulse); }), \
    FM("addTorque", [](PhysicsComponent* self, const DXSM::Vector3& inTorque){ self->AddTorque(inTorque); }), \
    FM("addAngularImpulse", [](PhysicsComponent* self, const DXSM::Vector3& inAngularImpulse){ self->AddAngularImpulse(inAngularImpulse); }), \
    FM("getAccumulatedForce", [](PhysicsComponent* self){ return self->GetAccumulatedForce(); }), \
    FM("getAccumulatedTorque", [](PhysicsComponent* self){ return self->GetAccumulatedTorque(); }), \
    FM("getAngularVelocity", [](PhysicsComponent* self){ return self->GetAngularVelocity(); }), \
    FM("getLinearVelocity", [](PhysicsComponent* self){ return self->GetLinearVelocity(); }), \
    FM("getPointVelocity", [](PhysicsComponent* self, const DXSM::Vector3& pt){ return self->GetPointVelocity(pt); }), \
    FM("getPosition", [](PhysicsComponent* self){ return self->GetPosition(); }), \
    FM("getRotation", [](PhysicsComponent* self){ return self->GetRotation(); }), \
    FM("resetForce", [](PhysicsComponent* self){ self->ResetForce(); }), \
    FM("resetTorque", [](PhysicsComponent* self){ self->ResetTorque(); }), \
    FM("setAngularVelocity", [](PhysicsComponent* self, const DXSM::Vector3& v){ self->SetAngularVelocity(v); }), \
    FM("setLinearVelocity", [](PhysicsComponent* self, const DXSM::Vector3& v){ self->SetLinearVelocity(v); }), \
    FM("setActive", [](PhysicsComponent* self, bool active){ self->SetActive(active); }), \
    FM("isActive", [](PhysicsComponent* self){ return self->IsActive(); })
#endif
