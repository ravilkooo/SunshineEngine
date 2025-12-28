#pragma once

// EASTL
#include <EASTL/vector.h>
#include <EASTL/hash_map.h>
#include <EASTL/shared_ptr.h>

// Engine
#include "AI/Perception/PerceptionComponent.h"
#include <Scene.h>


// Handles registration of teams and propagation of sensory events
// This system acts as a global hub that connects all AI agents through perception channels
class PerceptionSystem
{
    //friend class ***;
    friend class PerceptionComponent;

public:
    // --- TEAMS ---
    // Registers a new team in the perception system.
    // Id Unique team identifier.
    bool RegisterTeam(uint32_t Id);
    bool UnregisterTeam(uint32_t Id);
    //

    // --- TARGETS AND SOURCES ---
    bool AddSightTargetTeamIDsInTeam(uint32_t TeamId, eastl::vector<uint32_t>& InSightTargetTeamIDs);
    bool AddHearingSourceTeamIDsInTeam(uint32_t TeamId, eastl::vector<uint32_t>& InHearingSourceTeamIDs);

    bool RemoveSightTargetTeamIDsInTeam(uint32_t TeamId, eastl::vector<uint32_t>& InSightTargetTeamIDs);
    bool RemoveHearingSourceTeamIDsInTeam(uint32_t TeamId, eastl::vector<uint32_t>& InHearingSourceTeamIDs);

    bool ClearSightTargetTeamIDsInTeam(uint32_t TeamId);
    bool ClearHearingSourceTeamIDsInTeam(uint32_t TeamId);
    //
    
    // --- PERCEPTION COMPONENTS ---
    bool AddToTeam(uint32_t TeamId, PerceptionComponent* PC);
    bool RemoveFromTeam(uint32_t TeamId, PerceptionComponent* PC);
    bool ClearTeam(uint32_t TeamId);
    //

    static PerceptionSystem& Get()
    {
        static PerceptionSystem instance;
        return instance;
    }

    // Lua-friendly wrappers
    bool Lua_AddSightTargetTeamIDsInTeam(uint32_t TeamId, const std::vector<uint32_t>& ids);
    bool Lua_AddHearingSourceTeamIDsInTeam(uint32_t TeamId, const std::vector<uint32_t>& ids);
    bool Lua_RemoveSightTargetTeamIDsInTeam(uint32_t TeamId, const std::vector<uint32_t>& ids);
    bool Lua_RemoveHearingSourceTeamIDsInTeam(uint32_t TeamId, const std::vector<uint32_t>& ids);
private:

    // Represents a team participating in the perception system
    // Each team contains perceivers (GameObject with perception components)
    // and lists of other team IDs that THEY CAN SEE and WHO CAN HEAR THEM
    struct TeamSctruct
    {
        eastl::vector<PerceptionComponent*> Perceivers;

        eastl::vector<uint32_t> SightTargetTeamIDs;
        eastl::vector<uint32_t> HearingSourceTeamIDs;
    };

    void SetScene(eastl::shared_ptr<Scene> InScene) { SceneSP = InScene; };

    // --- RUNTIME ---
    // Checks line of sight between teams and perceivers.
    // Called automatically by the engine update loop.
    void CheckSights();

    /// Reports a noise event to all hearing teams.
    bool ReportNoise(PerceptionComponent* SourcePC, float Loudness);
    //


    eastl::shared_ptr<Scene> SceneSP;

    eastl::hash_map<uint32_t, TeamSctruct> Teams;
};

#ifndef PERCEPTIONSYSTEM_LUA_METHODS_APPLY
#define PERCEPTIONSYSTEM_LUA_METHODS_APPLY(FM) \
    FM("registerTeam", &PerceptionSystem::RegisterTeam) , \
    FM("unregisterTeam", &PerceptionSystem::UnregisterTeam) , \
    FM("addSightTargetTeamIDsInTeam", &PerceptionSystem::Lua_AddSightTargetTeamIDsInTeam) , \
    FM("addHearingSourceTeamIDsInTeam", &PerceptionSystem::Lua_AddHearingSourceTeamIDsInTeam) , \
    FM("removeSightTargetTeamIDsInTeam", &PerceptionSystem::Lua_RemoveSightTargetTeamIDsInTeam) , \
    FM("removeHearingSourceTeamIDsInTeam", &PerceptionSystem::Lua_RemoveHearingSourceTeamIDsInTeam) , \
    FM("clearSightTargetTeamIDsInTeam", &PerceptionSystem::ClearSightTargetTeamIDsInTeam) , \
    FM("clearHearingSourceTeamIDsInTeam", &PerceptionSystem::ClearHearingSourceTeamIDsInTeam) , \
    FM("addToTeam", &PerceptionSystem::AddToTeam) , \
    FM("removeFromTeam", &PerceptionSystem::RemoveFromTeam) , \
    FM("clearTeam", &PerceptionSystem::ClearTeam)
#endif
