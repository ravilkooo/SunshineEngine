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
#include <Physics/PhysicsEnums.h>
#include <Utils/UUID.h>

class TransformComponent;

class PhysicsSystem;

class PhysicsComponent :
    public Component
{
    friend class PhysicsComponent_Info;
public:

    PhysicsComponent() = default;
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
    
    // Setters for configuration before adding body
    void SetObjecUUID(SE::UUID objectUUID);
    void SetObjectLayer(JPH::ObjectLayer layer);
    void SetPosition(const JPH::RVec3& pos);
    void SetOrientation(const JPH::Quat& rot);
    void SetMotionType(JPH::EMotionType type);
    void SetActivation(JPH::EActivation activation);
    void SetShape(JPH::ShapeRefC shapePtr);

    // Create and add body to physics system
    void CreateBody(eastl::shared_ptr<PhysicsSystem> physicsSystem);

    void InitTransforms(TransformComponent* tc);

    JPH::Body* GetBody() const;
    JPH::BodyID GetBodyID() const;


private:
    SE::UUID m_objectUUID;
    JPH::Body* m_joltBody;
    JPH::BodyID m_joltBodyId;

    // Configuration properties before creating body
    JPH::RVec3 m_position = JPH::RVec3::sZero();
    JPH::Quat m_orientation = JPH::Quat::sIdentity();
    JPH::EMotionType m_motionType = JPH::EMotionType::Static;
    JPH::EActivation m_activation = JPH::EActivation::Activate;
    JPH::ObjectLayer m_objectLayer = Layers::NON_MOVING; // default layer
    JPH::ShapeRefC m_shape = nullptr;

    // To-do: make weak_ptr or simple ptr?
    eastl::shared_ptr<PhysicsSystem> m_physicsSystem;
    void FromJson(const json& j) override;
};

class PhysicsComponent_Info :
    public Component_Info
{
public:
    static const SE::ComponentType s_componentType = SE::ComponentType::PHYSICS;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    const std::type_info& getType() const override {
        return typeid(PhysicsComponent_Info);
    }

    bool IsAssigned() override { return false; }

    SE::CollisionShape m_shape = SE::CollisionShape::Box;
    SE::PhysicsMotionType m_motion = SE::PhysicsMotionType::Dynamic;
    SE::PhysicsActivation m_activation = SE::PhysicsActivation::Activate;
    CollisionLayer m_collisionLayer = {};

    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;
};