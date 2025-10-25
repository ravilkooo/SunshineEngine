#include "AI/Perception/PerceptionSystem.h"

#include <iostream>


void PerceptionSystem::Update()
{
    for (auto& [TeamId, Team] : Teams)
    {
        for (auto* Perceiver : Team.Perceivers)
        {
            if (!Perceiver)
                continue;

            if (!Perceiver->CanSee)
                continue;

            //Perceiver->GetSightSettings().SightRange;
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
        std::cerr << "[Warning]\n";
        return;
    }

    if (TeamId == UINT32_MAX)
    {
        std::cerr << "[Warning]\n";
        return;
    }

    if (Loudness <= 0.0f)
    {
        std::cerr << "[Warning]\n";
        return;
    }

    for (auto& [OtherTeamId, Team] : Teams)
    {
        for (auto* perceiver : Team.Perceivers)
        {
            if (!perceiver)
                continue;

            if (!perceiver->CanHear)
                continue;

            //Perceiver->GetHearingSettings().HearingRange;
        }
    }
}

void PerceptionSystem::ReportDamage(PerceptionComponent* Instigator, PerceptionComponent* Target, float DamageAmount)
{
    if (!Instigator || !Target)
    {
        std::cerr << "[Warning]\n";
        return;
    }
}
