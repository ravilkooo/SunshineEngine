#include "AI/Perception/PerceptionSystem.h"

// Engine
#include <Component/TransformComponent.h>
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>
#include <Physics/PhysicsSystem.h>

// C++
#include <iostream>

// Eastl
#include <EASTL/vector.h>


// ------------------------------------------------------------------------------------------------------
// ---------------------------------- TEAMS
// ------------------------------------------------------------------------------------------------------

bool PerceptionSystem::RegisterTeam(uint32_t Id)
{
    if (Id == UINT32_MAX)
    {
        //std::cerr << "[Warning] PerceptionSystem::RegisterTeam: Team ID is UINT32_MAX.\n";
        return false;
    }

    if (Teams.find(Id) != Teams.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::RegisterTeam: Team ID " << Id << " already exists.\n";
        return false;
    }  

    Teams.emplace(Id, TeamSctruct{});

    return true;
}

bool PerceptionSystem::UnregisterTeam(uint32_t Id)
{
    if (Id == UINT32_MAX)
    {
        //std::cerr << "[Warning] PerceptionSystem::UnregisterTeam: Team ID is UINT32_MAX.\n";
        return false;
    }

    auto it = Teams.find(Id);

    if (it == Teams.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::UnregisterTeam: Team ID " << Id << " not found.\n";
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

    Team.Perceivers.clear();
    Team.SightTargetTeamIDs.clear();
    Team.HearingSourceTeamIDs.clear();

    Teams.erase(it);

    return true;
}




// ------------------------------------------------------------------------------------------------------
// ---------------------------------- TARGETS AND SOURCES
// ------------------------------------------------------------------------------------------------------

bool PerceptionSystem::AddSightTargetTeamIDsInTeam(uint32_t TeamId, uint32_t InSightTargetTeamID)
{
    auto It = Teams.find(TeamId);

    if (It == Teams.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::AddSightTargetTeamIDsInTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    auto& Targets = It->second.SightTargetTeamIDs;

    auto itTarg = std::find(Targets.begin(), Targets.end(), InSightTargetTeamID);

    if (itTarg != Targets.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::AddSightTargetTeamIDsInTeam: Team " << TeamId << " already contains SightTargetTeamID = " << InSightTargetTeamID << ".\n";
        return false;
    }

    Targets.push_back(InSightTargetTeamID);

    return true;
}

bool PerceptionSystem::AddHearingSourceTeamIDsInTeam(uint32_t TeamId, uint32_t InHearingSourceTeamID)
{
    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::AddSightTargetTeamIDsInTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    auto& Sources = it->second.HearingSourceTeamIDs;

    auto itSrc = std::find(Sources.begin(), Sources.end(), InHearingSourceTeamID);

    if (itSrc != Sources.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::AddSightTargetTeamIDsInTeam: Team " << TeamId << " already contains HearingSourceTeamID = " << InHearingSourceTeamID << ".\n";
        return false;
    }

    Sources.push_back(InHearingSourceTeamID);

    return true;
}

bool PerceptionSystem::RemoveSightTargetTeamIDsInTeam(uint32_t TeamId, uint32_t InSightTargetTeamID)
{
    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::RemoveSightTargetTeamIDsInTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    auto& Targets = it->second.SightTargetTeamIDs;

    auto itTarg = std::find(Targets.begin(), Targets.end(), InSightTargetTeamID);

    if (itTarg == Targets.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::RemoveSightTargetTeamIDsInTeam: Team " << TeamId << " does not contain SightTargetTeamID = " << InSightTargetTeamID << ".\n";
        return false;
    }

    Targets.erase(itTarg);

    return true;
}

bool PerceptionSystem::RemoveHearingSourceTeamIDsInTeam(uint32_t TeamId, uint32_t InHearingSourceTeamID)
{
    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::RemoveHearingSourceTeamIDsInTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    auto& Sources = it->second.HearingSourceTeamIDs;

    auto itSrc = std::find(Sources.begin(), Sources.end(), InHearingSourceTeamID);

    if (itSrc == Sources.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::RemoveHearingSourceTeamIDsInTeam: Team " << TeamId << " does not contain SightTargetTeamID = " << InHearingSourceTeamID << ".\n";
        return false;
    }

    Sources.erase(itSrc);

    return true;
}

bool PerceptionSystem::ClearSightTargetTeamIDsInTeam(uint32_t TeamId)
{
    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::ClearSightTargetTeamIDsInTeam: Team ID " << TeamId << " not found.\n";
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
        //std::cerr << "[Warning] PerceptionSystem::ClearHearingSourceTeamIDsInTeam: Team ID " << TeamId << " not found.\n";
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
        //std::cerr << "[Warning] PerceptionSystem::AddToTeam: PerceptionComponent is null.\n";
        return false;
    }

    if (TeamId == UINT32_MAX)
    {
        //std::cerr << "[Warning] PerceptionSystem::AddToTeam: Team ID is UINT32_MAX.\n";
        return false;
    }

    if (PC->TeamId != UINT32_MAX)
    {
        //std::cerr << "[Warning] PerceptionSystem::AddToTeam: PerceptionComponent already in another team with id " << PC->TeamId << ".\n";
        return false;
    }

    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::AddToTeam: Team ID " << TeamId << " not found.\n";
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
        //std::cerr << "[Warning] PerceptionSystem::AddToTeam: PerceptionComponent already in team.\n";
    }

    return true;
}

bool PerceptionSystem::RemoveFromTeam(PerceptionComponent* PC)
{
    if (!PC)
    {
        //std::cerr << "[Warning] PerceptionSystem::RemoveFromTeam: PerceptionComponent is null.\n";
        return false;
    }

    if (PC->TeamId == UINT32_MAX)
    {
        //std::cerr << "[Warning] PerceptionSystem::RemoveFromTeam: Team ID is UINT32_MAX.\n";
        return false;
    }

    TeamSctruct& Team = Teams.find(PC->TeamId)->second;

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
    if (TeamId == UINT32_MAX)
    {
        //std::cerr << "[Warning] PerceptionSystem::ClearTeam: Team ID is UINT32_MAX.\n";
        return false;
    }

    auto it = Teams.find(TeamId);

    if (it == Teams.end())
    {
        //std::cerr << "[Warning] PerceptionSystem::ClearTeam: Team ID " << TeamId << " not found.\n";
        return false;
    }

    TeamSctruct& Team = it->second;

    for (PerceptionComponent* PC : Team.Perceivers)
    {
        if (PC)
        {
            PC->TeamId = UINT32_MAX;
        }
    }

    Team.Perceivers.clear();
    Team.SightTargetTeamIDs.clear();
    Team.HearingSourceTeamIDs.clear();

    return true;
}



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- RUNTIME
// ------------------------------------------------------------------------------------------------------

void PerceptionSystem::CheckSights(PhysicsSystem* PS)
{
    for (auto& [TeamId, Team] : Teams)
    {
        for (PerceptionComponent* ViewerPC : Team.Perceivers)
        {
            if (!ViewerPC || !ViewerPC->CanSee)
            {
                continue;
            }

            auto ViewerGO = Scene::GetInstance().GetGameObjectByUUID(ViewerPC->GetOwnerID());

            if (!ViewerGO || !ViewerGO->HasComponent<TransformComponent>())
            {
                continue;
            }

            auto ViewerTC = ViewerGO->GetComponent<TransformComponent>();

            DXSM::Vector3 ViewerPos = ViewerTC->GetPosition() + ViewerPC->EyesOffset;

            DXSM::Vector3 z_plus = DXSM::Vector3(0.0f, 0.0f, 1.0f);
            const auto wMat = ViewerTC->GetWorldMatrix();
            DXSM::Matrix A = wMat;
            A._41 = 0; A._42 = 0; A._43 = 0; A._44 = 1;
            const auto wMatInvTranspose = (A.Invert()).Transpose();

            DXSM::Vector3 ViewerForward = DXSM::Vector3::Transform(z_plus, wMatInvTranspose);
            ViewerForward.Normalize();

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

                    auto TargetGO = Scene::GetInstance().GetGameObjectByUUID(TargetPC->GetOwnerID());

                    if (!TargetGO || !TargetGO->HasComponent<TransformComponent>())
                    {
                        continue;
                    }

                    auto TargetTC = TargetGO->GetComponent<TransformComponent>();
                    DXSM::Vector3 TargetPos = TargetTC->GetPosition();

                    DXSM::Vector3 Dir = TargetPos - ViewerPos;
                    float Dist = Dir.Length();

                    DXSM::Vector3 DirNorm = Dir;
                    DirNorm.Normalize();

                    float ViewYaw = std::acosf(ViewerForward.Dot(DirNorm));
                    ViewYaw = ViewYaw * (ViewerForward.Cross(DirNorm).y > 0 ? 1 : -1);
                    
					// printf("ViewFwd: %.2f, %.2f, %.2f\t Direction: %.2f, %.2f, %.2f\n",
                    //     ViewerForward.x, ViewerForward.y, ViewerForward.z,
                    //     Dir.x, Dir.y, Dir.z);

                    bool WasVisible = std::find(ViewerPC->GOCanSee.begin(), ViewerPC->GOCanSee.end(), 
                        TargetPC->GetOwnerID()) != ViewerPC->GOCanSee.end();

                    if (ViewerPC->CanSeeThroughObjects)
                    {
                        if (WasVisible)
                        {
                            if (Dist > ViewerPC->LoseRadius || acosf(ViewerForward.Dot(DirNorm)) * 57.2958f > ViewerPC->FieldOfView * 0.5f)
                            {
                                ViewerPC->ChangeInSight(TargetPC->OwnerID, false);
                            }
                        }
                        else
                        {
                            if (Dist <= ViewerPC->SightRadius && acosf(ViewerForward.Dot(DirNorm)) * 57.2958f <= ViewerPC->FieldOfView * 0.5f)
                            {
                                ViewerPC->ChangeInSight(TargetPC->OwnerID, true, ViewYaw);
                            }
                        }
                    }
                    else
                    {
                        if (WasVisible)
                        {
                            if (Dist > ViewerPC->LoseRadius || acosf(ViewerForward.Dot(DirNorm)) * 57.2958f > ViewerPC->FieldOfView * 0.5f)
                            {
                                ViewerPC->ChangeInSight(TargetPC->OwnerID, false);

                                continue;
                            }
                        }
                        else
                        {
                            if (Dist > ViewerPC->SightRadius || acosf(ViewerForward.Dot(DirNorm)) * 57.2958f > ViewerPC->FieldOfView * 0.5f)
                            {
                                continue;
                            }
                        }

                        eastl::vector<SE::UUID> Ignore;
                        Ignore.reserve(2);
                        Ignore.push_back(ViewerPC->OwnerID);
                        // Ignore.push_back(TargetPC->OwnerID);

                        SE::UUID HitUUID = SE::UUID(0u);

                        bool HitSMTH = PS->RayCast(JPH::RVec3(ViewerPos.x, ViewerPos.y, ViewerPos.z),
                            JPH::Vec3(DirNorm.x, DirNorm.y, DirNorm.z),
                            Dist, Ignore, &HitUUID);

						// printf("\tHitSMTH: (%d); Hit UUID: (%s)\n", HitSMTH, HitUUID.ToString().c_str());

                        if (!HitSMTH)
                        {
                            if (!WasVisible)
                            {
                                ViewerPC->ChangeInSight(TargetPC->OwnerID, true, ViewYaw);
                            }
						}
                        else
                        {
                            if (HitUUID == SE::UUID(0u))
                            {
                                if (!WasVisible)
                                {
                                    ViewerPC->ChangeInSight(TargetPC->OwnerID, true, ViewYaw);
                                }
                            }
                            else if (HitUUID == TargetPC->OwnerID)
                            {
                                if (!WasVisible)
                                {
                                    ViewerPC->ChangeInSight(TargetPC->OwnerID, true, ViewYaw);
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

    auto SourceObj = Scene::GetInstance().GetGameObjectByUUID(SourcePC->GetOwnerID());

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

            auto Obj = Scene::GetInstance().GetGameObjectByUUID(SourcePC->GetOwnerID());

            if (!Obj->HasComponent<TransformComponent>())
            {
                continue;
            }

            auto ObjTC = Obj->GetComponent<TransformComponent>();

            if (perceiver->HearingRadius > (ObjTC->GetPosition() - SourceTC->GetPosition()).Length())
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



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- LUA BINDING
// ------------------------------------------------------------------------------------------------------

#define PS_ADD_FIELD(name) #name, &PerceptionSystem::name
#define PS_FIELD_PAIRS 

#define PS_ADD_METHOD(k, fn) k, fn
#define PS_METHOD_PAIRS PERCEPTIONSYSTEM_LUA_METHODS_APPLY(PS_ADD_METHOD)

LUA_REGISTER_TYPE(PerceptionSystem, "PerceptionSystem", PS_FIELD_PAIRS, PS_METHOD_PAIRS)

#undef PS_ADD_METHOD
#undef PS_FIELD_PAIRS
