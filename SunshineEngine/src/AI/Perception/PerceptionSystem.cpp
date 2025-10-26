#include "AI/Perception/PerceptionSystem.h"

#include <iostream>


void PerceptionSystem::CheckSights()
{
    for (auto& [TeamId, Team] : Teams)
    {
        for (auto* Perceiver : Team.Perceivers)
        {
            if (!Perceiver)
                continue;

            if (!Perceiver->CanSee)
                continue;

            //Perceiver->GetSightSettings();
        }
    }
}

void PerceptionSystem::RegisterTeam(uint32_t Id)
{
    if (Id == UINT32_MAX)
    {
        std::cerr << "[Warning]\n";
        return;
    }

    if (Teams.find(Id) != Teams.end())
    {
        std::cerr << "[Warning]\n";
        return;
    }  

    Teams.emplace(Id, TeamSctruct{});
}

void PerceptionSystem::UnregisterTeam(uint32_t Id)
{
    if (Id == UINT32_MAX)
    {
        std::cerr << "[Warning]\n";
        return;
    }

    auto it = Teams.find(Id);

    if (it == Teams.end())
    {
        std::cerr << "[Warning]\n";
        return;
    }
    
    Teams.erase(it);
}

void PerceptionSystem::AddToTeam(PerceptionComponent* Perception, uint32_t TeamId)
{
    if (!Perception)
    {
        std::cerr << "[Warning]\n";
        return;
    }

    if (TeamId == UINT32_MAX)
    {
        std::cerr << "[Warning]\n";
        return;
    }

    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        std::cerr << "[Warning]\n";
        return;
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
}

void PerceptionSystem::RemoveFromTeam(PerceptionComponent* Perception, uint32_t TeamId)
{
    if (!Perception)
    {
        std::cerr << "[Warning]\n";
        return;
    }

    if (TeamId == UINT32_MAX)
    {
        std::cerr << "[Warning]\n";
        return;
    }

    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        std::cerr << "[Warning]\n";
        return;
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
}

void PerceptionSystem::ReportNoise(PerceptionComponent* Source, uint32_t TeamId, float Loudness)
{
    if (!Source)
    {
        std::cerr << "[Warning] Source is null\n";
        return;
    }

    if (TeamId == UINT32_MAX)
    {
        std::cerr << "[Warning] TeamId is invalid\n";
        return;
    }

    if (Loudness <= 0.0f)
    {
        std::cerr << "[Warning] Loudness must be positive\n";
        return;
    }

    auto itSourceTeam = Teams.find(TeamId);

    if (itSourceTeam == Teams.end())
    {
        std::cerr << "[Warning] Source team not found\n";
        return;
    }

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

            // In progress
            if (HS.HearingRadius > 0)
            {
                auto NewLoudness = Loudness* HS.Sensitivity;

                if (NewLoudness >= HS.Threshold)
                {
                    // In progress
                    bool Location = true;
                    perceiver->Heard(Location, NewLoudness);
                }
            }
        }
    }
}
