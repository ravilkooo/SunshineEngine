#pragma once

#include <EASTL/vector.h>
#include <EASTL/functional.h>

#include <Utils/UUID.h>
#include <SimpleMath.h>
#include <Component/Component.h>


namespace DXSM = DirectX::SimpleMath;


struct SightStruct
{
    // Maximum distance at which the object can see others
    float SightRadius = 1000.0f; 
    // Distance at which sight of a target is lost
    float LoseRadius = 1000.0f;    
    // Angle of vision in degrees
    float FieldOfView = 90.0f;  
    // Whether vision penetrates obstacles
    bool CanSeeThroughObjects = false;       
};

struct HearingStruct
{
    // Maximum distance to hear sounds
    float HearingRadius = 1000.0f;  
    // Minimum loudness to trigger hearing
    float Threshold = 0.0f;     
    // Multiplier for loudness perception
    float Sensitivity = 1.0f;                
};


// Perception component attached to a GameObject
// Handles sight, hearing, and damage events
class PerceptionComponent : public Component
{
    friend class PerceptionSystem;

public:
    explicit PerceptionComponent(Sunshine::UUID Id): OwnerId(Id) {}

    Sunshine::UUID GetOwnerId() const { return OwnerId; }


    // --- Sight settings ---
    void SetSightStruct(SightStruct NewSightStruct);
    const SightStruct& GetSightSettings() const { return SightSettings; };

    void ChangeSightRadius(float NewRange);
    void ChangeLoseRadius(float NewLoseRadius);
    void ChangeFieldOfView(float NewFieldOfView);
    void ChangeCanSeeThroughObjects(bool NewCanSeeThroughObjects) { 
        SightSettings.CanSeeThroughObjects = NewCanSeeThroughObjects; };


    // --- Hearing settings ---
    void SetHearingStruct(HearingStruct NewHearingStruct);
    const HearingStruct& GetHearingSettings() const { return HearingSettings; };

    void ChangeHearingRange(float NewHearingRange);
    void ChangeThreshold(float NewThreshold);
    void ChangeSensitivity(float NewSensitivity);


    // --- Sight listeners ---
    using SightCallback = eastl::function<void(Sunshine::UUID TargetId)>;
    struct CallbackWrapperSight { uint64_t Id; SightCallback Callback; };

    uint64_t AddSightCallback(SightCallback Callback);
    void RemoveSightCallback(uint64_t Id);
    void ClearSightCallbacks() { SightCallbacks.clear(); };

    // --- Hearing listeners ---
    using HearingCallback = eastl::function<void(DXSM::Vector3 Location, float Loudness)>;
    struct CallbackWrapperHearing { uint64_t Id; HearingCallback Callback; };

    uint64_t AddHearingCallback(HearingCallback Callback);
    void RemoveHearingCallback(uint64_t Id);
    void ClearHearingCallbacks() { HearingCallbacks.clear(); };

    // --- Damage listeners ---
    using DamageCallback = eastl::function<void(Sunshine::UUID, float DamageAmount)>;
    struct CallbackWrapperDamage { uint64_t Id; DamageCallback Callback; };

    uint64_t AddDamageCallback(DamageCallback Callback);
    void RemoveDamageCallback(uint64_t Id);
    void ClearDamageCallbacks() { DamageCallbacks.clear(); };

  
    // --- Event triggers ---
    // Trigger a noise event to be broadcast globally
    bool MakeNoise(float Loudness);

    //
    //void ChangeInSight();

    // Notify listeners that a sound was heard
    void Heard(DXSM::Vector3& Location, float Loudness);

    // Notify listeners that damage was received
    void DealDamage(PerceptionComponent* Instigator, float DamageAmount);


    bool CanSee = false;
    bool CanHear = false;

private:
    void SetTeamId(uint32_t NewId) { TeamId = NewId; }


    Sunshine::UUID OwnerId;

    SightStruct SightSettings;
    HearingStruct HearingSettings;

    uint32_t TeamId = UINT32_MAX;

    eastl::vector<CallbackWrapperSight> SightCallbacks;
    eastl::vector<CallbackWrapperHearing> HearingCallbacks;
    eastl::vector<CallbackWrapperDamage> DamageCallbacks;

    uint64_t NextCallbackId = 1u;
};
