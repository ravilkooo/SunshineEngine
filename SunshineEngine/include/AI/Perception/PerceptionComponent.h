#pragma once

// Eastl
#include <EASTL/unique_ptr.h>

// Ñ++
#include <unordered_map>

// Engine
#include <Utils/UUID.h>
#include <SimpleMath.h>
#include <Component/Component.h>

// Lua
#include <sol/sol.hpp>

// Json
#include <nlohmann/json.hpp>


class PerceptionSystem;


namespace DXSM = DirectX::SimpleMath;


using json = nlohmann::json;


// Perception component attached to a GameObject
// Handles sight, hearing, and damage events
class PerceptionComponent : public Component
{
    friend class PerceptionSystem;
    friend class PerceptionComponent_Info;

public:
    explicit PerceptionComponent(SE::UUID GOID) : OwnerID(GOID) {}

    ~PerceptionComponent();

    SE::UUID GetOwnerID() const { return OwnerID; }


    // --- SIGHT ---
    void SetCanSee(bool NewCondition) { CanSee = NewCondition; }
    bool SetSight(float NewSightRadius, float NewLoseRadius, float NewFieldOfView, DXSM::Vector3 NewEyesOffset, bool NewCanSeeThroughObjects);

    bool SetSightRadius(float NewSightRadius);
    bool SetLoseRadius(float NewLoseRadius);
    bool SetFieldOfView(float NewFieldOfView);
    void SetEyesOffset(DXSM::Vector3 NewEyesOffset) { EyesOffset = NewEyesOffset; };
    void SetCanSeeThroughObjects(bool NewCanSeeThroughObjects) { CanSeeThroughObjects = NewCanSeeThroughObjects; };

    bool GetCanSee() { return CanSee; }
    float GetSightRadius() { return SightRadius; };
    float GetLoseRadius() { return LoseRadius; };
    float GetFieldOfView() { return FieldOfView; };
    DXSM::Vector3 GetEyesOffset() { return EyesOffset; };
    bool GetCanSeeThroughObjects() { return CanSeeThroughObjects; };
    //

    // --- HEARING ---
    void SetCanHear(bool NewCondition) { CanHear = NewCondition; }
    bool SetHearing(float NewHearingRadius, float NewThreshold, float NewSensitivity);

    bool SetHearingRadius(float NewHearingRadius);
    bool SetThreshold(float NewThreshold);
    bool SetSensitivity(float NewSensitivity);

    bool GetCanHear() { return CanHear; }
    float GetHearingRadius() { return HearingRadius; };
    float GetThreshold() { return Threshold; };
    float GetSensitivity() { return Sensitivity; };
    //

    // --- SIGHT LISTENERS ---
    uint64_t AddSightCallback(const sol::function& Callback);
    void RemoveSightCallback(uint64_t Id) { SightCallbacks.erase(Id); };
    void ClearSightCallbacks() { SightCallbacks.clear(); };
    //

    // --- HEARING LISTENERS ---
    uint64_t AddHearingCallback(const sol::function& Callback);
    void RemoveHearingCallback(uint64_t Id) { HearingCallbacks.erase(Id); };
    void ClearHearingCallbacks() { HearingCallbacks.clear(); };
    //

    // --- DAMAGE LISTENERS ---
    uint64_t AddDamageCallback(const sol::function& Callback);
    void RemoveDamageCallback(uint64_t Id) { DamageCallbacks.erase(Id); };
    void ClearDamageCallbacks() { DamageCallbacks.clear(); };
    //

    // --- EVENTS ---
    // Trigger a noise event to be broadcast globally
    bool MakeNoise(float Loudness);

    // Notify listeners that damage was received
    void DealDamage(PerceptionComponent* Instigator, float DamageAmount);
    //

    const std::type_info& getType() const override { return typeid(PerceptionComponent); }
    static const SE::ComponentType s_componentType = SE::ComponentType::PERCEPTION;
    const SE::ComponentType ComponentType() const override { return s_componentType; }

    void FromJson(const json& j) override;

private:
    //
    void ChangeInSight(SE::UUID GOID, bool NewCondition, float InYaw = 0.0f);

    // Notify listeners that a sound was heard
    void Heard(SE::UUID GOID, float Loudness);


    SE::UUID OwnerID;

    // --- SIGHT ---
    bool CanSee = false;

    // Eyes relative to the object's position
    DXSM::Vector3 EyesOffset = DXSM::Vector3::Zero;

    // Maximum distance at which the object can see others
    float SightRadius = 0.0f;

    // Distance at which sight of a target is lost
    float LoseRadius = 0.0f;

    // Angle of vision in degrees
    float FieldOfView = 90.0f;

    // Whether vision penetrates obstacles
    bool CanSeeThroughObjects = false;
    //

    // --- Hearing ---
    bool CanHear = false;

    // Maximum distance to hear sounds
    float HearingRadius = 0.0f;

    // Minimum loudness to trigger hearing
    float Threshold = 0.0f;

    // Multiplier for loudness perception
    float Sensitivity = 1.0f;
    //

    uint32_t TeamId = UINT32_MAX;

    std::unordered_map<uint64_t, sol::function> SightCallbacks;
    std::unordered_map<uint64_t, sol::function> HearingCallbacks;
    std::unordered_map<uint64_t, sol::function> DamageCallbacks;

    uint64_t NextCallbackId = 1u;

    std::vector<SE::UUID> GOCanSee;
};



class PerceptionComponent_Info : public Component_Info
{
public:
    explicit PerceptionComponent_Info() { };

    static const SE::ComponentType s_componentType = SE::ComponentType::PERCEPTION;

    const SE::ComponentType ComponentType() const override { return s_componentType; }
    const std::type_info& getType() const override { return typeid(PerceptionComponent_Info); }
    bool IsAssigned() override { return true; }

    void SetSightRadius(float NewSightRadius);
    void SetLoseRadius(float NewLoseRadius);
    void SetFieldOfView(float NewFieldOfView);

    void SetHearingRadius(float NewHearingRadius);
    void SetThreshold(float NewThreshold);
    void SetSensitivity(float NewSensitivity);

    json ToJson() const override;
    void FromJson(const json& j) override;


    bool CanSee = false;
    DXSM::Vector3 EyesOffset = DXSM::Vector3::Zero;
    float SightRadius = 0.0f;
    float LoseRadius = 0.0f;
    float FieldOfView = 90.0f;
    bool CanSeeThroughObjects = false;
    bool CanHear = false;
    float HearingRadius = 0.0f;
    float Threshold = 0.0f;
    float Sensitivity = 1.0f;
};



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- LUA
// ------------------------------------------------------------------------------------------------------

#ifndef PERCEPTIONCOMPONENT_LUA_METHODS_APPLY
#define PERCEPTIONCOMPONENT_LUA_METHODS_APPLY(FM) \
    FM("setCanSee",                 &PerceptionComponent::SetCanSee) , \
    FM("setSight",                  &PerceptionComponent::SetSight) , \
    FM("setSightRadius",            &PerceptionComponent::SetSightRadius) , \
    FM("setLoseRadius",             &PerceptionComponent::SetLoseRadius) , \
    FM("setFieldOfView",            &PerceptionComponent::SetFieldOfView) , \
    FM("setEyesOffset",             &PerceptionComponent::SetEyesOffset) , \
    FM("setCanSeeThroughObjects",   &PerceptionComponent::SetCanSeeThroughObjects) , \
    \
    FM("getCanSee",                 &PerceptionComponent::GetCanSee) , \
    FM("getSightRadius",            &PerceptionComponent::GetSightRadius) , \
    FM("getLoseRadius",             &PerceptionComponent::GetLoseRadius) , \
    FM("getFieldOfView",            &PerceptionComponent::GetFieldOfView) , \
    FM("getEyesOffset",             &PerceptionComponent::GetEyesOffset) , \
    FM("getCanSeeThroughObjects",   &PerceptionComponent::GetCanSeeThroughObjects) , \
    \
    FM("setCanHear",                &PerceptionComponent::SetCanHear) , \
    FM("setHearing",                &PerceptionComponent::SetHearing) , \
    FM("setHearingRadius",          &PerceptionComponent::SetHearingRadius) , \
    FM("setThreshold",              &PerceptionComponent::SetThreshold) , \
    FM("setSensitivity",            &PerceptionComponent::SetSensitivity) , \
    \
    FM("getCanHear",                &PerceptionComponent::GetCanHear) , \
    FM("getHearingRadius",          &PerceptionComponent::GetHearingRadius) , \
    FM("getThreshold",              &PerceptionComponent::GetThreshold) , \
    FM("getSensitivity",            &PerceptionComponent::GetSensitivity) , \
    \
    FM("addSightCallback",          &PerceptionComponent::AddSightCallback) , \
    FM("removeSightCallback",       &PerceptionComponent::RemoveSightCallback) , \
    FM("clearSightCallbacks",       &PerceptionComponent::ClearSightCallbacks) , \
    \
    FM("addHearingCallback",        &PerceptionComponent::AddHearingCallback) , \
    FM("removeHearingCallback",     &PerceptionComponent::RemoveHearingCallback) , \
    FM("clearHearingCallbacks",     &PerceptionComponent::ClearHearingCallbacks) , \
    \
    FM("addDamageCallback",         &PerceptionComponent::AddDamageCallback) , \
    FM("removeDamageCallback",      &PerceptionComponent::RemoveDamageCallback) , \
    FM("clearDamageCallbacks",      &PerceptionComponent::ClearDamageCallbacks) , \
    \
    FM("makeNoise",                 &PerceptionComponent::MakeNoise), \
    FM("dealDamage",                &PerceptionComponent::DealDamage)
#endif
