#pragma once

#include <EASTL/vector.h>

class GameObject;


struct SightStruct
{
    float SightRange = 1000.0f;
};

struct HearingStruct
{
    float HearingRange = 500.0f;
};

class PerceptionComponent
{
    friend class PerceptionSystem;

public:
    PerceptionComponent(GameObject* GO): Owner(GO) {}

    GameObject* GetOwner() const { return Owner; }

    void SetSightStruct(SightStruct NewSightStruct);
    const SightStruct& GetSightSettings() const { return SightSettings; };
    void ChangeSightRange(float NewSightRange);

    void SetHearingStruct(HearingStruct NewHearingStruct);
    const HearingStruct& GetHearingSettings() const { return HearingSettings; };
    void ChangeHearingRange(float NewHearingRange);

    void MakeNoise(float Loudness);
    void DealDamage(PerceptionComponent* Source, float DamageAmount);

    bool CanSee = false;
    bool CanHear = false;

private:
    void SetTeamId(uint32_t NewId) { TeamId = NewId; }

    GameObject* Owner = nullptr;

    SightStruct SightSettings;

    HearingStruct HearingSettings;

    uint32_t TeamId = UINT32_MAX;
};
