#pragma once

// EASTL
#include <EASTL/hash_map.h>
#include <EASTL/functional.h>

// Engine
#include <Utils/UUID.h>
#include <SimpleMath.h>
#include <Component/Component.h>

// Lua
#include <sol/sol.hpp>


class PerceptionSystem;


namespace DXSM = DirectX::SimpleMath;


// Perception component attached to a GameObject
// Handles sight, hearing, and damage events
class PerceptionComponent : public Component
{
    friend class PerceptionSystem;

public:
    explicit PerceptionComponent(SE::UUID GOID): OwnerID(GOID) {}

    ~PerceptionComponent();

    SE::UUID GetOwnerID() const { return OwnerID; }


    // --- SIGHT ---
    bool SetSight(float NewSightRadius, float NewLoseRadius, float NewFieldOfView, bool NewCanSeeThroughObjects);

    bool SetSightRadius(float NewSightRadius);
    bool SetLoseRadius(float NewLoseRadius);
    bool SetFieldOfView(float NewFieldOfView);
    void SetCanSeeThroughObjects(bool NewCanSeeThroughObjects) { CanSeeThroughObjects = NewCanSeeThroughObjects; };

    float GetSightRadius() { return SightRadius; };
    float GetLoseRadius() { return LoseRadius; };
    float GetFieldOfView() { return FieldOfView; };
    bool GetCanSeeThroughObjects() { return CanSeeThroughObjects; };
    //

    // --- HEARING ---
    bool SetHearing(float NewHearingRadius, float NewThreshold, float NewSensitivity);

    bool SetHearingRadius(float NewHearingRadius);
    bool SetThreshold(float NewThreshold);
    bool SetSensitivity(float NewSensitivity);

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


    bool CanSee = false;
    bool CanHear = false;

private:
    //
    void ChangeInSight(SE::UUID GOID, bool NewCondition);

    // Notify listeners that a sound was heard
    void Heard(SE::UUID GOID, float Loudness);


    SE::UUID OwnerID;

    // --- SIGHT ---
    // Maximum distance at which the object can see others
    float SightRadius = 1000.0f;

    // Distance at which sight of a target is lost
    float LoseRadius = 1000.0f;

    // Angle of vision in degrees
    float FieldOfView = 90.0f;

    // Whether vision penetrates obstacles
    bool CanSeeThroughObjects = false;
    //

    // --- Hearing ---
    // Maximum distance to hear sounds
    float HearingRadius = 1000.0f;

    // Minimum loudness to trigger hearing
    float Threshold = 0.0f;

    // Multiplier for loudness perception
    float Sensitivity = 1.0f;
    //

    uint32_t TeamId = UINT32_MAX;

    eastl::hash_map<uint64_t, sol::function> SightCallbacks;
    eastl::hash_map<uint64_t, sol::function> HearingCallbacks;
    eastl::hash_map<uint64_t, sol::function> DamageCallbacks;

    uint64_t NextCallbackId = 1u;

    eastl::vector<SE::UUID> GOCanSee;
};


class PerceptionComponent_Info : public Component_Info
{
public:
    PerceptionComponent_Info() { };

    static const SE::ComponentType s_componentType = SE::ComponentType::PERCEPTION;

    const SE::ComponentType ComponentType() const override { return s_componentType; }
    const std::type_info& getType() const override { return typeid(PerceptionComponent_Info); }
    bool IsAssigned() override { return true; }


    PerceptionComponent* Component;
};


// ------------------------------------------------------------------------------------------------------
// ---------------------------------- LUA
// ------------------------------------------------------------------------------------------------------

#ifndef PERCEPTIONCOMPONENT_LUA_METHODS_APPLY
#define PERCEPTIONCOMPONENT_LUA_METHODS_APPLY(FM) \
    /* OWNER */ \
    FM("getOwnerId", [](PerceptionComponent* self) { return self->GetOwnerID(); }) \
    \
    /* SIGHT */ \
  , FM("setSight", [](PerceptionComponent* self, float r, float lose, float fov, bool through) { return self->SetSight(r, lose, fov, through); }) \
  , FM("setSightRadius", [](PerceptionComponent* self, float r) { return self->SetSightRadius(r); }) \
  , FM("setLoseRadius", [](PerceptionComponent* self, float r) { return self->SetLoseRadius(r); }) \
  , FM("setFieldOfView", [](PerceptionComponent* self, float f) { return self->SetFieldOfView(f); }) \
  , FM("setCanSeeThroughObjects", [](PerceptionComponent* self, bool b) { self->SetCanSeeThroughObjects(b); }) \
  , FM("getSightRadius", [](PerceptionComponent* self) { return self->GetSightRadius(); }) \
  , FM("getLoseRadius", [](PerceptionComponent* self) { return self->GetLoseRadius(); }) \
  , FM("getFieldOfView", [](PerceptionComponent* self) { return self->GetFieldOfView(); }) \
  , FM("getCanSeeThroughObjects", [](PerceptionComponent* self) { return self->GetCanSeeThroughObjects(); }) \
    \
    /* HEARING */ \
  , FM("setHearing", [](PerceptionComponent* self, float r, float th, float sens) { return self->SetHearing(r, th, sens); }) \
  , FM("setHearingRadius", [](PerceptionComponent* self, float r) { return self->SetHearingRadius(r); }) \
  , FM("setThreshold", [](PerceptionComponent* self, float t) { return self->SetThreshold(t); }) \
  , FM("setSensitivity", [](PerceptionComponent* self, float s) { return self->SetSensitivity(s); }) \
  , FM("getHearingRadius", [](PerceptionComponent* self) { return self->GetHearingRadius(); }) \
  , FM("getThreshold", [](PerceptionComponent* self) { return self->GetThreshold(); }) \
  , FM("getSensitivity", [](PerceptionComponent* self) { return self->GetSensitivity(); }) \
    \
    /* SIGHT CALLBACKS */ \
  , FM("addSightCallback", [](PerceptionComponent* self, const sol::function& fn) { return self->AddSightCallback(fn); }) \
  , FM("removeSightCallback", [](PerceptionComponent* self, uint64_t id) { self->RemoveSightCallback(id); }) \
  , FM("clearSightCallbacks", [](PerceptionComponent* self) { self->ClearSightCallbacks(); }) \
    \
    /* HEARING CALLBACKS */ \
  , FM("addHearingCallback", [](PerceptionComponent* self, const sol::function& fn) { return self->AddHearingCallback(fn); }) \
  , FM("removeHearingCallback", [](PerceptionComponent* self, uint64_t id) { self->RemoveHearingCallback(id); }) \
  , FM("clearHearingCallbacks", [](PerceptionComponent* self) { self->ClearHearingCallbacks(); }) \
    \
    /* DAMAGE CALLBACKS */ \
  , FM("addDamageCallback", [](PerceptionComponent* self, const sol::function& fn) { return self->AddDamageCallback(fn); }) \
  , FM("removeDamageCallback", [](PerceptionComponent* self, uint64_t id) { self->RemoveDamageCallback(id); }) \
  , FM("clearDamageCallbacks", [](PerceptionComponent* self) { self->ClearDamageCallbacks(); }) \
    \
    /* EVENTS */ \
  , FM("makeNoise", [](PerceptionComponent* self, float loudness) { return self->MakeNoise(loudness); }) \
  , FM("dealDamage", [](PerceptionComponent* self, PerceptionComponent* instigator, float damage) { self->DealDamage(instigator, damage); })
#endif
