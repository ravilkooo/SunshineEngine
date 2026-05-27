#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include <Component/Component.h>
#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;

class PhysicsSystem;
class TransformComponent;

class CharacterControllerComponent : public Component
{
public:
	CharacterControllerComponent();
	~CharacterControllerComponent();
	CharacterControllerComponent(PhysicsSystem* physicsSystem,
        TransformComponent* transformComp);

    void Initialize(PhysicsSystem* physicsSystem, TransformComponent* transformComp);
    
    bool Initialized = false;

    //
    // Jolt character
    //

    JPH::Ref<JPH::CharacterVirtual> Character;
    JPH::Ref<JPH::Shape> Shape;

    //
    // Runtime state
    //

    DXSM::Vector3 Velocity = DXSM::Vector3::Zero;

    bool Grounded = false;

    DXSM::Vector3 GroundNormal = DXSM::Vector3(0, 1, 0);

    //
    // Movement tuning
    //

    float MoveSpeed = 6.0f;

    float Acceleration = 30.0f;
    float AirAcceleration = 8.0f;

    float JumpSpeed = 7.5f;

    float Gravity = -24.0f;

    float MaxFallSpeed = -40.0f;

    bool EnableStickToFloor = false;
    bool EnableWalkStairs = false;


    //
    // Capsule
    //

    eastl::shared_ptr<SE::ColliderData> m_colliderData;
    // float Radius = 0.35f;
    // float Height = 1.8f;

    float StepHeight = 0.3f;

    float MaxSlopeAngle = 45.0f;

    // Inherited via Component
    const std::type_info& getType() const override {
        return typeid(CharacterControllerComponent);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::CHARACTER_CONTROLLER;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    void FromJson(const json& j) override;
    void FromJson(const json& j, PhysicsSystem* physicsSystem,
        TransformComponent* transformComp);
private:
    void DestroyCharacter();
};

class CharacterControllerComponent_Info :
    public Component_Info
{
public:

    // Inherited via Component
    const std::type_info& getType() const override {
        return typeid(CharacterControllerComponent_Info);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::CHARACTER_CONTROLLER;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    bool IsAssigned() override { return true; }
    eastl::unique_ptr<CharacterControllerComponent> m_assignedComponent;

    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;
};
