#include "AI/Perception/PerceptionSystem.h"

// Engine
#include <Component/TransformComponent.h>

// C++
#include <iostream>


// ------------------------------------------------------------------------------------------------------
// ---------------------------------- TEAMS
// ------------------------------------------------------------------------------------------------------

bool PerceptionSystem::RegisterTeam(uint32_t Id)
{
    if (Id == UINT32_MAX)
    {
        std::cerr << "[Warning] PerceptionSystem::RegisterTeam: Team ID is UINT32_MAX.\n";
        return false;
    }

    if (Teams.find(Id) != Teams.end())
    {
        std::cerr << "[Warning] PerceptionSystem::RegisterTeam: Team ID " << Id << " already exists.\n";
        return false;
    }  

    Teams.emplace(Id, TeamSctruct{});

    return true;
}

bool PerceptionSystem::UnregisterTeam(uint32_t Id)
{
    if (Id == UINT32_MAX)
    {
        std::cerr << "[Warning] PerceptionSystem::UnregisterTeam: Team ID is UINT32_MAX.\n";
        return false;
    }

    auto it = Teams.find(Id);

    if (it == Teams.end())
    {
        std::cerr << "[Warning] PerceptionSystem::UnregisterTeam: Team ID " << Id << " not found.\n";
        return false;
    }
    
    TeamSctruct& Team = it->second;

    for (PerceptionComponent* PC : Team.Perceivers)
    {
        if (PC == nullptr)
        {
            continue;
        }

        PC->TeamId = UINT32_MAX;
    }

    Teams.erase(it);

    return true;
}

// ------------------------------------------------------------------------------------------------------
// ---------------------------------- TARGETS AND SOURCES
// ------------------------------------------------------------------------------------------------------
bool PerceptionSystem::AddSightTargetTeamIDsInTeam(uint32_t TeamId, eastl::vector<uint32_t>& InSightTargetTeamIDs)
{
    auto It = Teams.find(TeamId);

    if (It == Teams.end())
    {
        std::cerr << "[Warning] PerceptionSystem::AddSightTargetTeamIDsInTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    auto& Targets = It->second.SightTargetTeamIDs;

    for (uint32_t TargetId : InSightTargetTeamIDs)
    {
        auto itTarg = eastl::find(Targets.begin(), Targets.end(), TargetId);

        if (itTarg != Targets.end())
        {
            std::cerr << "[Warning] PerceptionSystem::AddSightTargetTeamIDsInTeam: Team " << TeamId << " already contains SightTargetTeamID = " << TargetId << ".\n";
            continue;
        }

        Targets.push_back(TargetId);
    }

    return true;
}

bool PerceptionSystem::AddHearingSourceTeamIDsInTeam(uint32_t TeamId, eastl::vector<uint32_t>& InHearingSourceTeamIDs)
{
    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        std::cerr << "[Warning] PerceptionSystem::AddSightTargetTeamIDsInTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    auto& Sources = it->second.HearingSourceTeamIDs;

    for (uint32_t SourceId : InHearingSourceTeamIDs)
    {
        auto itSrc = eastl::find(Sources.begin(), Sources.end(), SourceId);

        if (itSrc != Sources.end())
        {
            std::cerr << "[Warning] PerceptionSystem::AddSightTargetTeamIDsInTeam: Team " << TeamId << " already contains HearingSourceTeamID = " << SourceId << ".\n";
            continue;
        }

        Sources.push_back(SourceId);
    }

    return true;
}

bool PerceptionSystem::RemoveSightTargetTeamIDsInTeam(uint32_t TeamId, eastl::vector<uint32_t>& InSightTargetTeamIDs)
{
    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        std::cerr << "[Warning] PerceptionSystem::RemoveSightTargetTeamIDsInTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    auto& Targets = it->second.SightTargetTeamIDs;

    for (uint32_t TargetId : InSightTargetTeamIDs)
    {
        auto itTarg = eastl::find(Targets.begin(), Targets.end(), TargetId);

        if (itTarg == Targets.end())
        {
            std::cerr << "[Warning] PerceptionSystem::RemoveSightTargetTeamIDsInTeam: Team " << TeamId << " does not contain SightTargetTeamID = " << TargetId << ".\n";
            continue;
        }

        Targets.erase(itTarg);
    }

    return true;
}

bool PerceptionSystem::RemoveHearingSourceTeamIDsInTeam(uint32_t TeamId, eastl::vector<uint32_t>& InHearingSourceTeamIDs)
{
    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        std::cerr << "[Warning] PerceptionSystem::RemoveHearingSourceTeamIDsInTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    auto& Sources = it->second.HearingSourceTeamIDs;

    for (uint32_t SourceId : InHearingSourceTeamIDs)
    {
        auto itSrc = eastl::find(Sources.begin(), Sources.end(), SourceId);

        if (itSrc == Sources.end())
        {
            std::cerr << "[Warning] PerceptionSystem::RemoveHearingSourceTeamIDsInTeam: Team " << TeamId << " does not contain SightTargetTeamID = " << SourceId << ".\n";
            continue;
        }

        Sources.erase(itSrc);
    }

    return true;
}

bool PerceptionSystem::ClearSightTargetTeamIDsInTeam(uint32_t TeamId)
{
    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        std::cerr << "[Warning] PerceptionSystem::ClearSightTargetTeamIDsInTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    it->second.SightTargetTeamIDs.clear();

    return true;
}

bool PerceptionSystem::ClearHearingSourceTeamIDsInTeam(uint32_t TeamId)
{
    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        std::cerr << "[Warning] PerceptionSystem::ClearHearingSourceTeamIDsInTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    it->second.HearingSourceTeamIDs.clear();

    return true;
}

// ------------------------------------------------------------------------------------------------------
// ---------------------------------- PERCEPTION COMPONENTS
// ------------------------------------------------------------------------------------------------------

bool PerceptionSystem::AddToTeam(uint32_t TeamId, PerceptionComponent* PC)
{
    if (!PC)
    {
        std::cerr << "[Warning] PerceptionSystem::AddToTeam: PerceptionComponent is null.\n";
        return false;
    }

    if (TeamId == UINT32_MAX)
    {
        std::cerr << "[Warning] PerceptionSystem::AddToTeam: Team ID is UINT32_MAX.\n";
        return false;
    }

    if (PC->TeamId != UINT32_MAX)
    {
        std::cerr << "[Warning] PerceptionSystem::AddToTeam: PerceptionComponent already in another team with id " << PC->TeamId << ".\n";
        return false;
    }

    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        std::cerr << "[Warning] PerceptionSystem::AddToTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    TeamSctruct& Team = it->second;

    bool AlreadyIn = false;

    for (auto* Perceiver : Team.Perceivers)
    {
        if (Perceiver == PC)
        {
            AlreadyIn = true;
            break;
        }
    }

    if (!AlreadyIn)
    {
        PC->TeamId = TeamId;
        Team.Perceivers.push_back(PC);
    }
    else
    {
        std::cerr << "[Warning] PerceptionSystem::AddToTeam: PerceptionComponent already in team.\n";
    }

    return true;
}

bool PerceptionSystem::RemoveFromTeam(uint32_t TeamId, PerceptionComponent* PC)
{
    if (!PC)
    {
        std::cerr << "[Warning] PerceptionSystem::RemoveFromTeam: PerceptionComponent is null.\n";
        return false;
    }

    if (TeamId == UINT32_MAX)
    {
        std::cerr << "[Warning] PerceptionSystem::RemoveFromTeam: Team ID is UINT32_MAX.\n";
        return false;
    }

    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        std::cerr << "[Warning] PerceptionSystem::RemoveFromTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    TeamSctruct& Team = it->second;

    for (auto itP = Team.Perceivers.begin(); itP != Team.Perceivers.end(); ++itP)
    {
        if (*itP == PC)
        {
            PC->TeamId = UINT32_MAX;
            Team.Perceivers.erase(itP);

            break;
        }
    }

    return true;
}

bool PerceptionSystem::ClearTeam(uint32_t TeamId)
{
    return false;
}

// ------------------------------------------------------------------------------------------------------
// ---------------------------------- RUNTIME
// ------------------------------------------------------------------------------------------------------

void PerceptionSystem::CheckSights()
{
    for (auto& [TeamId, Team] : Teams)
    {
        for (PerceptionComponent* ViewerPC : Team.Perceivers)
        {
            if (!ViewerPC || !ViewerPC->CanSee)
            {
                continue;
            }

            auto ViewerGO = SceneSP->GetGameObjectByUUID(ViewerPC->GetOwnerID());

            if (!ViewerGO || !ViewerGO->HasComponent<TransformComponent>())
            {
                continue;
            }

            auto ViewerTC = ViewerGO->GetComponent<TransformComponent>();

            DXSM::Vector3 ViewerPos = ViewerTC->m_position;
            DXSM::Vector3 ViewerForward; //= ViewerTC->m_forward;

            for (uint32_t TargetTeamId : Team.SightTargetTeamIDs)
            {
                auto itTargetTeam = Teams.find(TargetTeamId);

                if (itTargetTeam == Teams.end())
                {
                    continue;
                }

                for (PerceptionComponent* TargetPC : itTargetTeam->second.Perceivers)
                {
                    if (!TargetPC)
                    {
                        continue;
                    }

                    auto TargetGO = SceneSP->GetGameObjectByUUID(TargetPC->GetOwnerID());

                    if (!TargetGO || !TargetGO->HasComponent<TransformComponent>())
                    {
                        continue;
                    }

                    auto TargetTC = TargetGO->GetComponent<TransformComponent>();
                    DXSM::Vector3 TargetPos = TargetTC->m_position;

                    DXSM::Vector3 Dir = TargetPos - ViewerPos;
                    float Dist = Dir.Length();

                    DXSM::Vector3 DirNorm = Dir;
                    DirNorm.Normalize();

                    bool WasVisible = eastl::find(ViewerPC->GOCanSee.begin(), ViewerPC->GOCanSee.end(), TargetPC->GetOwnerID()) != ViewerPC->GOCanSee.end();

                    if (WasVisible)
                    {
                        if (Dist > ViewerPC->LoseRadius)
                        {
                            ViewerPC->ChangeInSight(TargetPC->OwnerID, false);

                            continue;
                        }

                        if (acosf(ViewerForward.Dot(DirNorm)) * 57.2958f > ViewerPC->FieldOfView * 0.5f)
                        {
                            ViewerPC->ChangeInSight(TargetPC->OwnerID, false);

                            continue;
                        }
                    }                 

                    bool HitTarget = false;

                    if (ViewerPC->CanSeeThroughObjects)
                    {
                        HitTarget = true;
                    }
                    else
                    {
                        //
                    }

                    if (HitTarget)
                    {
                        if (!WasVisible)
                        {
                            ViewerPC->ChangeInSight(TargetPC->OwnerID, true);
                        }                  
                    }
                    else
                    {
                        if (WasVisible)
                        {
                            ViewerPC->ChangeInSight(TargetPC->OwnerID, false);
                        }
                    }
                }
            }
        }
    }
}

bool PerceptionSystem::ReportNoise(PerceptionComponent* SourcePC, float Loudness)
{
    if (!SourcePC)
    {
        std::cerr << "[Warning] PerceptionSystem::ReportNoise: SourcePerceptionComponent is null.\n";
        return false;
    }

    if (SourcePC->TeamId == UINT32_MAX)
    {
        std::cerr << "[Warning] PerceptionSystem::ReportNoise: SourcePerceptionComponent not in team.\n";
        return false;
    }

    if (Loudness <= 0.0f)
    {
        std::cerr << "[Warning] PerceptionSystem::ReportNoise: Loudness must be positive.\n";
        return false;
    }

    auto SourceObj = SceneSP->GetGameObjectByUUID(SourcePC->GetOwnerID());

    if (!SourceObj->HasComponent<TransformComponent>())
    {
        std::cerr << "[Warning] PerceptionSystem::ReportNoise: SourcePerceptionComponent has no TransformComponent\n";
        return false;
    }

    auto SourceTC = SourceObj->GetComponent<TransformComponent>();
    const TeamSctruct& SourceTeam = Teams.find(SourcePC->TeamId)->second;

    for (uint32_t HearingTeamId : SourceTeam.HearingSourceTeamIDs)
    {
        auto itHearingTeam = Teams.find(HearingTeamId);

        if (itHearingTeam == Teams.end())
        {
            continue;
        }

        TeamSctruct& HearingTeam = itHearingTeam->second;

        for (auto* perceiver : HearingTeam.Perceivers)
        {
            if (!perceiver)
            {
                continue;
            }

            if (!perceiver->CanHear)
            {
                continue;
            }

            auto Obj = SceneSP->GetGameObjectByUUID(SourcePC->GetOwnerID());

            if (!Obj->HasComponent<TransformComponent>())
            {
                continue;
            }

            auto ObjTC = Obj->GetComponent<TransformComponent>();

            if (perceiver->HearingRadius > (ObjTC->m_position - SourceTC->m_position).Length())
            {
                auto NewLoudness = Loudness* perceiver->Sensitivity;

                if (NewLoudness >= perceiver->Threshold)
                {
                    perceiver->Heard(SourcePC->OwnerID, NewLoudness);
                }
            }
        }
    }

    return true;
}
