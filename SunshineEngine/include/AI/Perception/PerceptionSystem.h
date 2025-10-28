#pragma once

#include <EASTL/vector.h>
#include <EASTL/hash_map.h>
#include <EASTL/shared_ptr.h>

#include "AI/Perception/PerceptionComponent.h"
#include <Scene.h>


// Represents a team participating in the perception system
// Each team contains perceivers (GameObject with perception components)
// and lists of other team IDs that THEY CAN SEE and WHO CAN HEAR THEM
struct TeamSctruct
{
    eastl::vector<PerceptionComponent*> Perceivers;

    eastl::vector<uint32_t> SightTargetTeamIDs;
    eastl::vector<uint32_t> HearingSourceTeamIDs;
};


// Handles registration of teams and propagation of sensory events
// This system acts as a global hub that connects all AI agents through perception channels
class PerceptionSystem
{
public:
    static PerceptionSystem& Get()
    {
        static PerceptionSystem instance;
        return instance;
    }

    void SetScene(eastl::shared_ptr<Scene> S) { SceneSP = S; };

    // Checks line of sight between teams and perceivers.
    // Called automatically by the engine update loop.
    void CheckSights();

    // Registers a new team in the perception system.
    // Id Unique team identifier.
    bool RegisterTeam(uint32_t Id);
    bool UnregisterTeam(uint32_t Id);

    // Clean all nullptr perceivers
    void CleanupInvalidPerceiversInTeam(uint32_t Id);
    void CleanupInvalidPerceivers();

    bool AddToTeam(PerceptionComponent* Perception, uint32_t TeamId);
    bool RemoveFromTeam(PerceptionComponent* Perception, uint32_t TeamId);

    /// Reports a noise event to all hearing teams.
    bool ReportNoise(PerceptionComponent* Source, uint32_t TeamId, float Loudness);

private:
    eastl::shared_ptr<Scene> SceneSP;

    eastl::hash_map<uint32_t, TeamSctruct> Teams;
};


