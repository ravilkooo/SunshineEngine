#pragma once
#include <Component/Component.h>
#include <EASTL/unique_ptr.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/EActivation.h>

#include <Physics/CollisionUtils.h>
#include <Utils/UUID.h>

class PhysicsSystem;

class PhysicsComponent :
    public Component
{
    friend class PhysicsComponent_Info;
public:

    PhysicsComponent() = default;
    ~PhysicsComponent() = default;

    PhysicsComponent(const PhysicsComponent&) = delete;
    PhysicsComponent& operator=(const PhysicsComponent&) = delete;

    PhysicsComponent(PhysicsComponent&&) noexcept = default;
    PhysicsComponent& operator=(PhysicsComponent&&) noexcept = default;

    const std::type_info& getType() const override {
        return typeid(PhysicsComponent);
    }
    
    // Setters for configuration before adding body
    void SetObjecUUID(Sunshine::UUID objectUUID);
    void SetObjectLayer(JPH::ObjectLayer layer);
    void SetPosition(const JPH::RVec3& pos);
    void SetOrientation(const JPH::Quat& rot);
    void SetMotionType(JPH::EMotionType type);
    void SetActivation(JPH::EActivation activation);
    void SetShape(JPH::ShapeRefC shapePtr);

    // Create and add body to physics system
    void CreateBody(eastl::shared_ptr<PhysicsSystem> physicsSystem);

    JPH::Body* GetBody() const;
    JPH::BodyID GetBodyID() const;


private:
    Sunshine::UUID m_objectUUID;
    JPH::Body* m_joltBody;
    JPH::BodyID m_joltBodyId;

    // Configuration properties before creating body
    JPH::ObjectLayer m_objectLayer = Layers::NON_MOVING; // default layer
    JPH::RVec3 m_position = JPH::RVec3::sZero();
    JPH::Quat m_orientation = JPH::Quat::sIdentity();
    JPH::EMotionType m_motionType = JPH::EMotionType::Static;
    JPH::EActivation m_activation = JPH::EActivation::Activate;
    JPH::ShapeRefC m_shape = nullptr;

    eastl::shared_ptr<PhysicsSystem> m_physicsSystem;
};

class PhysicsComponent_Info :
    public Component_Info
{
public:
    static ComponentType StaticComponentType() {
        return ComponentType::PHYSICS;
    }

    const std::type_info& getType() const override {
        return typeid(PhysicsComponent_Info);
    }

    bool IsAssigned() override { return false; }

    CollisionLayer m_collisionLayer;
};