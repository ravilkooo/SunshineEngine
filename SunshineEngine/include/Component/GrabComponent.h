#pragma once

#include <EASTL/shared_ptr.h>

#include <Component/Component.h>
#include <Utils/UUID.h>

class PhysicsConstraint;

class GrabComponent : public Component
{
public:
    GrabComponent() = default;
    ~GrabComponent() = default;

    GrabComponent(const GrabComponent&) = delete;
    GrabComponent& operator=(const GrabComponent&) = delete;

    GrabComponent(GrabComponent&&) noexcept = default;
    GrabComponent& operator=(GrabComponent&&) noexcept = default;

    //
    // Gameplay
    //

    float m_holdDistance = 2.0f;
    float m_maxGrabDistance = 3.0f;
    float m_throwImpulse = 10.0f;

    bool m_rotateWithCamera = true;
    bool m_canGrabDynamicBodies = true;
    bool m_canGrabKinematicBodies = false;

    SE::UUID GrabbedObject;

    eastl::shared_ptr<PhysicsConstraint> Constraint;

    // Don't need it?
    bool IsGrabbing() const
    {
        return false;
        // return GrabbedObject.IsValid();
    }

    //
    // Optional
    //

    // eastl::string AttachmentSocket = "RightHand";

    //
    // Inherited via Component
    //

    const std::type_info& getType() const override {
        return typeid(GrabComponent);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::GRAB;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    void FromJson(const json& j) override;
};

class GrabComponent_Info :
    public Component_Info
{
public:
    GrabComponent_Info();
    ~GrabComponent_Info() = default;

    GrabComponent_Info(const GrabComponent_Info&) = delete;
    GrabComponent_Info& operator=(const GrabComponent_Info&) = delete;

    GrabComponent_Info(GrabComponent_Info&&) noexcept = default;
    GrabComponent_Info& operator=(GrabComponent_Info&&) noexcept = default;

    // Inherited via Component
    const std::type_info& getType() const override {
        return typeid(GrabComponent_Info);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::GRAB;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    bool IsAssigned() override { return true; }
    eastl::unique_ptr<GrabComponent> m_assignedComponent;

    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;
};