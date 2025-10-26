#pragma once

#include <EASTL/vector.h>
#include <EASTL/functional.h>


class GameObject;


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
class PerceptionComponent
{
    friend class PerceptionSystem;

public:
    PerceptionComponent(GameObject* GO): Owner(GO) {}

    GameObject* GetOwner() const { return Owner; }


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
    using SightCallback = eastl::function<void(GameObject* Target)>;
    struct ListenerWrapperSight { uint64_t Id; SightCallback Func; };

    uint64_t AddSightListener(SightCallback Callback);
    void RemoveSightListener(uint64_t Id);
    void ClearSightListeners() { SightListeners.clear(); };

    // --- Hearing listeners ---
    using HearingCallback = eastl::function<void(bool& Location, float Loudness)>;
    struct ListenerWrapperHearing { uint64_t Id; HearingCallback Func; };

    uint64_t AddHearingListener(HearingCallback Callback);
    void RemoveHearingListener(uint64_t Id);
    void ClearHearingListeners() { HearingListeners.clear(); };

    // --- Damage listeners ---
    using DamageCallback = eastl::function<void(GameObject* Source, float DamageAmount)>;
    struct ListenerWrapperDamage { uint64_t Id; DamageCallback Func; };

    uint64_t AddDamageListener(DamageCallback Callback);
    void RemoveDamageListener(uint64_t Id);
    void ClearDamageListeners() { DamageListeners.clear(); };

    // --- Event triggers ---
    // Trigger a noise event to be broadcast globally
    void MakeNoise(float Loudness);

    //void ChangeInSight();

    // Notify listeners that a sound was heard
    void Heard(bool& Location, float Loudness);

    // Notify listeners that damage was received
    void DealDamage(PerceptionComponent* Instigator, float DamageAmount);


    bool CanSee = false;
    bool CanHear = false;

private:
    void SetTeamId(uint32_t NewId) { TeamId = NewId; }


    GameObject* Owner = nullptr;

    SightStruct SightSettings;
    HearingStruct HearingSettings;

    uint32_t TeamId = UINT32_MAX;

    eastl::vector<ListenerWrapperSight> SightListeners;
    eastl::vector<ListenerWrapperHearing> HearingListeners;
    eastl::vector<ListenerWrapperDamage> DamageListeners;

    uint64_t NextListenerId = 1u;
};
