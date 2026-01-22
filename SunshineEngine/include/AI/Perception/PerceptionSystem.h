#pragma once

// EASTL
#include <EASTL/vector.h>
#include <EASTL/hash_map.h>
#include <EASTL/shared_ptr.h>

// C++
#include <vector>
#include <memory>
#include <unordered_map>

// Engine
#include "AI/Perception/PerceptionComponent.h"
#include <Scene.h>


class PhysicsSystem;


// Handles registration of teams and propagation of sensory events
// This system acts as a global hub that connects all AI agents through perception channels
class PerceptionSystem
{
    friend class Game;
    friend class PerceptionComponent;

public:
    static PerceptionSystem& Get()
    {
        static PerceptionSystem instance;
        return instance;
    }

    // --- TEAMS ---
    // Registers a new team in the perception system.
    // Id Unique team identifier.
    bool RegisterTeam(uint32_t Id);
    bool UnregisterTeam(uint32_t Id);
    //

    // --- TARGETS AND SOURCES ---
    bool AddSightTargetTeamIDsInTeam(uint32_t TeamId, uint32_t InSightTargetTeamID);
    bool AddHearingSourceTeamIDsInTeam(uint32_t TeamId, uint32_t InHearingSourceTeamID);

    bool RemoveSightTargetTeamIDsInTeam(uint32_t TeamId, uint32_t InSightTargetTeamID);
    bool RemoveHearingSourceTeamIDsInTeam(uint32_t TeamId, uint32_t InHearingSourceTeamID);

    bool ClearSightTargetTeamIDsInTeam(uint32_t TeamId);
    bool ClearHearingSourceTeamIDsInTeam(uint32_t TeamId);
    //
    
    // --- PERCEPTION COMPONENTS ---
    bool AddToTeam(uint32_t TeamId, PerceptionComponent* PC);
    bool RemoveFromTeam(PerceptionComponent* PC);
    bool ClearTeam(uint32_t TeamId);
    //

    void Clear() { Teams.clear(); }

private:
    // Represents a team participating in the perception system
    // Each team contains perceivers (GameObject with perception components)
    // and lists of other team IDs that THEY CAN SEE and WHO CAN HEAR THEM
    struct TeamSctruct
    {
        std::vector<PerceptionComponent*> Perceivers;

        std::vector<uint32_t> SightTargetTeamIDs;
        std::vector<uint32_t> HearingSourceTeamIDs;
    };

    // --- RUNTIME ---
    // Checks line of sight between teams and perceivers.
    // Called automatically by the engine update loop.
    void CheckSights(PhysicsSystem* PhysicsSystemSP);

    /// Reports a noise event to all hearing teams.
    bool ReportNoise(PerceptionComponent* SourcePC, float Loudness);
    //


    std::unordered_map<uint32_t, TeamSctruct> Teams;
};



// --- LUA BINDING ---

#ifndef PERCEPTIONSYSTEM_LUA_METHODS_APPLY
#define PERCEPTIONSYSTEM_LUA_METHODS_APPLY(FM) \
    FM("registerTeam",                     &PerceptionSystem::RegisterTeam) , \
    FM("unregisterTeam",                   &PerceptionSystem::UnregisterTeam) , \
    \
    FM("addSightTargetTeamIDsInTeam",      &PerceptionSystem::AddSightTargetTeamIDsInTeam) , \
    FM("addHearingSourceTeamIDsInTeam",    &PerceptionSystem::AddHearingSourceTeamIDsInTeam) , \
    FM("removeSightTargetTeamIDsInTeam",   &PerceptionSystem::RemoveSightTargetTeamIDsInTeam) , \
    FM("removeHearingSourceTeamIDsInTeam", &PerceptionSystem::RemoveHearingSourceTeamIDsInTeam) , \
    FM("clearSightTargetTeamIDsInTeam",    &PerceptionSystem::ClearSightTargetTeamIDsInTeam) , \
    FM("clearHearingSourceTeamIDsInTeam",  &PerceptionSystem::ClearHearingSourceTeamIDsInTeam) , \
    \
    FM("addToTeam",                        &PerceptionSystem::AddToTeam) , \
    FM("removeFromTeam",                   &PerceptionSystem::RemoveFromTeam) , \
    FM("clearTeam",                        &PerceptionSystem::ClearTeam)
#endif
