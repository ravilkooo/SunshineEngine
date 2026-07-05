#pragma once

#include <Component/Component.h>
#include <Utils/UUID.h>

class CharacterComponent;
class TransformComponent;

class BouncePadComponent : public Component
{
public:
    BouncePadComponent() = default;
    BouncePadComponent(TransformComponent* tc);
    ~BouncePadComponent() = default;

    BouncePadComponent(const BouncePadComponent&) = delete;
    BouncePadComponent& operator=(const BouncePadComponent&) = delete;

    BouncePadComponent(BouncePadComponent&&) noexcept = default;
    BouncePadComponent& operator=(BouncePadComponent&&) noexcept = default;

    float m_minBounceVelocity = 15.0f;

    void BounceCharacter(SE::UUID characterUUID);
    void BounceCharacter(CharacterComponent* character);

    float GetMinBounceVelocity() const { return m_minBounceVelocity; }
    void SetMinBounceVelocity(float value) { m_minBounceVelocity = value; }

    // Inherited via Component
    const std::type_info& getType() const override {
        return typeid(BouncePadComponent);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::BOUNCE_PAD;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    void FromJson(const json& j) override;
    void FromJson(const json& j, TransformComponent* tc);

    TransformComponent* m_assignedTransform = nullptr;
};

class BouncePadComponent_Info : public Component_Info
{
public:
    BouncePadComponent_Info() = default;
    ~BouncePadComponent_Info() = default;

    BouncePadComponent_Info(const BouncePadComponent_Info&) = delete;
    BouncePadComponent_Info& operator=(const BouncePadComponent_Info&) = delete;

    BouncePadComponent_Info(BouncePadComponent_Info&&) noexcept = default;
    BouncePadComponent_Info& operator=(BouncePadComponent_Info&&) noexcept = default;

    float m_minBounceVelocity = 15.0f;

    // Inherited via Component
    const std::type_info& getType() const override {
        return typeid(BouncePadComponent_Info);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::BOUNCE_PAD;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    bool IsAssigned() override { return false; }

    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;
};

#ifndef BOUNCEPADCOMPONENT_LUA_PROPERTIES_APPLY
#define BOUNCEPADCOMPONENT_LUA_PROPERTIES_APPLY(FP) \
    FP(minBounceVelocity, &BouncePadComponent::GetMinBounceVelocity, &BouncePadComponent::SetMinBounceVelocity)
#endif

#ifndef BOUNCEPADCOMPONENT_LUA_METHODS_APPLY
#define BOUNCEPADCOMPONENT_LUA_METHODS_APPLY(FM) \
    FM("bounceCharacter", [](BouncePadComponent* self, CharacterComponent* character){ return self->BounceCharacter(character); }) \
    FM("bounceCharacterByUUID", [](BouncePadComponent* self, SE::UUIDhilo characterUUID){ return self->BounceCharacter(SE::UUID::FromHilo(characterUUID)); })
#endif
