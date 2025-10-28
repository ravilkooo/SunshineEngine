#include "AI/Perception/PerceptionSystem.h"

#include <Component/TransformComponent.h>

#ifdef _DEBUG
#include <iostream>
#endif

//In progress
void PerceptionSystem::CheckSights()
{
    //for (auto& [TeamId, Team] : Teams)
    //{
    //    for (auto* Perceiver : Team.Perceivers)
    //    {
    //        if (!Perceiver)
    //            continue;

    //        if (!Perceiver->CanSee)
    //            continue;
    //    }
    //}
}

bool PerceptionSystem::RegisterTeam(uint32_t Id)
{
    if (Id == UINT32_MAX)
    {
#ifdef _DEBUG
        std::cerr << "[Warning] Cannot register team: invalid ID (UINT32_MAX).\n";
#endif
        return false;
    }

    if (Teams.find(Id) != Teams.end())
    {
#ifdef _DEBUG
        std::cerr << "[Warning] Cannot register team: ID already exists.\n";
#endif
        return false;
    }  

    Teams.emplace(Id, TeamSctruct{});

    return true;
}

bool PerceptionSystem::UnregisterTeam(uint32_t Id)
{
    if (Id == UINT32_MAX)
    {
#ifdef _DEBUG
        std::cerr << "[Warning] Cannot unregister team: invalid ID (UINT32_MAX).\n";
#endif
        return false;
    }

    auto it = Teams.find(Id);

    if (it == Teams.end())
    {
#ifdef _DEBUG
        std::cerr << "[Warning] Cannot unregister team: ID not found.\n";
#endif
        return false;
    }
    
    Teams.erase(it);

    return true;
}

void PerceptionSystem::CleanupInvalidPerceiversInTeam(uint32_t Id)
{
    if (Id == UINT32_MAX)
    {
#ifdef _DEBUG
        std::cerr << "[Warning] CleanupInvalidPerceiversInTeam: invalid team ID (UINT32_MAX).\n";
#endif
        return;
    }

    auto it = Teams.find(Id);

    if (it == Teams.end())
    {
#ifdef _DEBUG
        std::cerr << "[Warning] CleanupInvalidPerceiversInTeam: team not found.\n";
#endif
        return;
    }

    TeamSctruct& Team = it->second;

#ifdef _DEBUG
    size_t SizeBefore = Team.Perceivers.size();
#endif

    Team.Perceivers.erase( eastl::remove(Team.Perceivers.begin(), Team.Perceivers.end(), nullptr), Team.Perceivers.end() );

#ifdef _DEBUG
    size_t removed = SizeBefore - Team.Perceivers.size();

    if (removed > 0)
        std::cout << "[Info] CleanupInvalidPerceiversInTeam: removed " << removed << " invalid perceivers from team " << Id << ".\n";
#endif
}

void PerceptionSystem::CleanupInvalidPerceivers()
{
    for (auto& [TeamId, _] : Teams)
        CleanupInvalidPerceiversInTeam(TeamId);

#ifdef _DEBUG
    std::cout << "[Info] CleanupInvalidPerceivers: all teams checked for invalid perceivers.\n";
#endif
}

bool PerceptionSystem::AddToTeam(PerceptionComponent* Perception, uint32_t TeamId)
{
    if (!Perception)
    {
#ifdef _DEBUG
        std::cerr << "[Warning] Cannot add to team: PerceptionComponent is null.\n";
#endif
        return false;
    }

    if (TeamId == UINT32_MAX)
    {
#ifdef _DEBUG
        std::cerr << "[Warning] Cannot add to team: invalid team ID.\n";
#endif
        return false;
    }

    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
#ifdef _DEBUG
        std::cerr << "[Warning] Cannot add to team: invalid team ID.\n";
#endif
        return false;
    }

    TeamSctruct& Team = it->second;

    bool AlreadyIn = false;

    for (auto* Perceiver : Team.Perceivers)
    {
        if (Perceiver == Perception)
        {
            AlreadyIn = true;
            break;
        }
    }

    if (!AlreadyIn)
    {
        Perception->SetTeamId(TeamId);
        Team.Perceivers.push_back(Perception);
    }  

    return true;
}

bool PerceptionSystem::RemoveFromTeam(PerceptionComponent* Perception, uint32_t TeamId)
{
    if (!Perception)
    {
#ifdef _DEBUG
        std::cerr << "[Warning] Cannot remove from team: PerceptionComponent is null.\n";
#endif
        return false;
    }

    if (TeamId == UINT32_MAX)
    {
#ifdef _DEBUG
        std::cerr << "[Warning] Cannot remove from team: PerceptionComponent is null.\n";
#endif
        return false;
    }

    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
#ifdef _DEBUG
        std::cerr << "[Warning] Cannot remove from team: team not found.\n";
#endif
        return false;
    }

    TeamSctruct& Team = it->second;

    for (auto itP = Team.Perceivers.begin(); itP != Team.Perceivers.end(); ++itP)
    {
        if (*itP == Perception)
        {
            Perception->SetTeamId(UINT32_MAX);
            Team.Perceivers.erase(itP);

            break;
        }
    }

    return true;
}

bool PerceptionSystem::ReportNoise(PerceptionComponent* Source, uint32_t TeamId, float Loudness)
{
    if (!Source)
    {
#ifdef _DEBUG
        std::cerr << "[Warning] ReportNoise failed: Source component is null.\n";
#endif
        return false;
    }

    if (TeamId == UINT32_MAX)
    {
#ifdef _DEBUG
        std::cerr << "[Warning] ReportNoise failed: Invalid team ID.\n";
#endif
        return false;
    }

    if (Loudness <= 0.0f)
    {
#ifdef _DEBUG
        std::cerr << "[Warning] ReportNoise failed: Loudness must be positive.\n";
#endif
        return false;
    }

    auto itSourceTeam = Teams.find(TeamId);

    if (itSourceTeam == Teams.end())
    {
#ifdef _DEBUG
        std::cerr << "[Warning] ReportNoise failed: Source team not found.\n";
#endif
        return false;
    }

    auto SourceObj = SceneSP->GetGameObjectByUUID(Source->GetOwnerId());

    if (!SourceObj->HasComponent<TransformComponent>())
    {
#ifdef _DEBUG
        std::cerr << "[Warning] Source has no TransformComponent\n";
#endif
        return false;
    }

    auto SourceTC = SourceObj->GetComponent<TransformComponent>();

    const TeamSctruct& SourceTeam = itSourceTeam->second;

    for (uint32_t HearingTeamId : SourceTeam.HearingSourceTeamIDs)
    {
        auto itHearingTeam = Teams.find(HearingTeamId);

        if (itHearingTeam == Teams.end())
            continue;

        TeamSctruct& HearingTeam = itHearingTeam->second;

        for (auto* perceiver : HearingTeam.Perceivers)
        {
            if (!perceiver)
                continue;

            if (!perceiver->CanHear)
                continue;

            auto HS = perceiver->GetHearingSettings();

            auto Obj = SceneSP->GetGameObjectByUUID(Source->GetOwnerId());

            if (!Obj->HasComponent<TransformComponent>())
                continue;

            auto ObjTC = Obj->GetComponent<TransformComponent>();

            if (HS.HearingRadius > (ObjTC->m_position - SourceTC->m_position).Length())
            {
                auto NewLoudness = Loudness* HS.Sensitivity;

                if (NewLoudness >= HS.Threshold)
                    perceiver->Heard(SourceTC->m_position, NewLoudness);
            }
        }
    }

    return true;
}
