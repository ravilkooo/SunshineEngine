#pragma once

#include <EASTL/vector.h>
#include <EASTL/hash_map.h>

#include "PerceptionComponent.h"


struct TeamSctruct
{
    eastl::vector<PerceptionComponent*> Perceivers;

    eastl::vector<uint32_t> HearingSourceTeamIDs;
    eastl::vector<uint32_t> SightTargetTeamIDs;
};

class PerceptionSystem
{
public:
    static PerceptionSystem& Get()
    {
        static PerceptionSystem instance;
        return instance;
    }

    void Update();

    void RegisterTeam(uint32_t Id);
    void UnregisterTeam(uint32_t Id);

    void AddToTeam(PerceptionComponent* Perception, uint32_t TeamId);
    void RemoveFromTeam(PerceptionComponent* Perception, uint32_t TeamId);

    void ReportNoise(PerceptionComponent* Source, uint32_t TeamId, float Loudness);
    void ReportDamage(PerceptionComponent* Instigator, PerceptionComponent* Target, float DamageAmount);

private:
    eastl::hash_map<uint32_t, TeamSctruct> Teams;
    eastl::vector<uint32_t> TeamIds;
};


