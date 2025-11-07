#pragma once
#include <Component/Component.h>
#include <EASTL/unique_ptr.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyID.h>

class PhysicsComponent :
    public Component
{

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
private:
    JPH::Body* joltBody;
    JPH::BodyID joltBodyId;
};

