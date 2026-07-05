#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>
#include <Component/Component.h>
#include <Utils/UUID.h>

#include <SimpleMath.h>

namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

class TransformComponent;

class MovingPlatformComponent : public Component
{
public:
    MovingPlatformComponent() = default;
    ~MovingPlatformComponent() = default;

    MovingPlatformComponent(const MovingPlatformComponent&) = delete;
    MovingPlatformComponent& operator=(const MovingPlatformComponent&) = delete;

    MovingPlatformComponent(MovingPlatformComponent&&) noexcept = default;
    MovingPlatformComponent& operator=(MovingPlatformComponent&&) noexcept = default;

    // DXSM::Vector3 m_previousPosition;
    bool m_affectCharacters = true;
    JPH::Vec3 m_velocity = JPH::Vec3::sZero();
    SE::UUID m_objectUUID;

    // Inherited via Component
    const std::type_info& getType() const override {
        return typeid(MovingPlatformComponent);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::MOVING_PLATFORM;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    void FromJson(const json& j) override;
private:
};

class MovingPlatformComponent_Info : public Component_Info
{
public:
    MovingPlatformComponent_Info() = default;
    ~MovingPlatformComponent_Info() = default;

    MovingPlatformComponent_Info(const MovingPlatformComponent_Info&) = delete;
    MovingPlatformComponent_Info& operator=(const MovingPlatformComponent_Info&) = delete;

    MovingPlatformComponent_Info(MovingPlatformComponent_Info&&) noexcept = default;
    MovingPlatformComponent_Info& operator=(MovingPlatformComponent_Info&&) noexcept = default;

    bool m_affectCharacters = true;

    // Inherited via Component
    const std::type_info& getType() const override {
        return typeid(MovingPlatformComponent_Info);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::MOVING_PLATFORM;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    bool IsAssigned() override { return false; }

    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;
};

#ifndef MOVING_PLATFORM_COMPONENT_LUA_FIELDS_APPLY
#define MOVING_PLATFORM_COMPONENT_LUA_FIELDS_APPLY(F) \
    F(m_affectCharacters)
#endif

#ifndef BOUNCEPADCOMPONENT_LUA_METHODS_APPLY
#define BOUNCEPADCOMPONENT_LUA_METHODS_APPLY(FM) \
    FM("bounceCharacter", [](MovingPlatformComponent* self, CharacterComponent* character){ return self->BounceCharacter(character); }) \
    FM("bounceCharacterByUUID", [](MovingPlatformComponent* self, SE::UUIDhilo characterUUID){ return self->BounceCharacter(SE::UUID::FromHilo(characterUUID)); })
#endif
