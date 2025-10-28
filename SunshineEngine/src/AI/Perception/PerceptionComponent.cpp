#include "AI/Perception/PerceptionComponent.h"

#include "AI/Perception/PerceptionSystem.h"

#ifdef _DEBUG
#include <iostream>
#endif

// ---------------- Sight ----------------
void PerceptionComponent::SetSightStruct(SightStruct NewSightSettings)
{
	if (NewSightSettings.SightRadius < 0 || NewSightSettings.LoseRadius < 0)
	{
#ifdef _DEBUG
		std::cerr << "[Warning] Negative sight values are invalid. SightRadius and LoseRadius must be >= 0.\n";
#endif
		return;
	}

	if (NewSightSettings.LoseRadius < NewSightSettings.SightRadius)
	{
#ifdef _DEBUG
		std::cerr << "[Warning] LoseRadius cannot be smaller than SightRadius.\n";
#endif
		return;
	}

	if (NewSightSettings.FieldOfView > 180 || NewSightSettings.FieldOfView < 0)
	{
#ifdef _DEBUG
		std::cerr << "[Warning] FieldOfView must be between 0 and 180 degrees.\n";
#endif
		return;
	}

	SightSettings = NewSightSettings;
}

void PerceptionComponent::ChangeSightRadius(float NewSightRaduis)
{
	if (NewSightRaduis < 0) 
	{
#ifdef _DEBUG
		std::cerr << "[Warning] SightRadius cannot be negative.\n";
#endif
		return;
	}

	if (NewSightRaduis < SightSettings.LoseRadius)
	{
#ifdef _DEBUG
		std::cerr << "[Warning] SightRadius is smaller than current LoseRadius. LoseRadius automatically adjusted.\n";
#endif
		
		SightSettings.LoseRadius = NewSightRaduis;
	}

	SightSettings.SightRadius = NewSightRaduis;
}

void PerceptionComponent::ChangeLoseRadius(float NewLoseRadius)
{
	if (NewLoseRadius < SightSettings.SightRadius)
	{
#ifdef _DEBUG
		std::cerr << "[Warning] LoseRadius cannot be smaller than SightRadius.\n";
#endif
		return;
	}

	SightSettings.LoseRadius = NewLoseRadius;
}

void PerceptionComponent::ChangeFieldOfView(float NewFieldOfView)
{
	if (NewFieldOfView > 180 || NewFieldOfView < 0)
	{
#ifdef _DEBUG
		std::cerr << "[Warning] FieldOfView must be between 0 and 180 degrees.\n";
#endif
		return;
	}

	SightSettings.FieldOfView = NewFieldOfView;
}

// ---------------- Hearing ----------------
void PerceptionComponent::SetHearingStruct(HearingStruct NewHearingSettings)
{
	if (NewHearingSettings.HearingRadius < 0)
	{
#ifdef _DEBUG
		std::cerr << "[Warning] HearingRadius cannot be negative.\n";
#endif
		return;
	}

	if (NewHearingSettings.Threshold < 0)
	{
#ifdef _DEBUG
		std::cerr << "[Warning] Threshold cannot be negative.\n";
#endif
		return;
	}

	if (NewHearingSettings.Sensitivity < 0)
	{
#ifdef _DEBUG
		std::cerr << "[Warning] Sensitivity cannot be negative.\n";
#endif
		return;
	}

	HearingSettings = NewHearingSettings;
}

void PerceptionComponent::ChangeHearingRange(float NewHearingRange)
{
	if (NewHearingRange < 0) 
	{
#ifdef _DEBUG
		std::cerr << "[Warning] HearingRadius cannot be negative.\n";
#endif
		return;
	}

	HearingSettings.HearingRadius = NewHearingRange;
}

void PerceptionComponent::ChangeThreshold(float NewThreshold)
{
	if (NewThreshold < 0)
	{
#ifdef _DEBUG
		std::cerr << "[Warning] Threshold cannot be negative.\n";
#endif
		return;
	}

	HearingSettings.Threshold = NewThreshold;
}

void PerceptionComponent::ChangeSensitivity(float NewSensitivity)
{
	if (NewSensitivity < 0)
	{
#ifdef _DEBUG
		std::cerr << "[Warning] Sensitivity cannot be negative.\n";
#endif
		return;
	}

	HearingSettings.Sensitivity = NewSensitivity;
}

// ---------------- Callbacks ----------------
uint64_t PerceptionComponent::AddSightCallback(SightCallback Callback)
{
	CallbackWrapperSight CWS { NextCallbackId++, Callback };
	SightCallbacks.push_back(CWS);

	return CWS.Id;
}

void PerceptionComponent::RemoveSightCallback(uint64_t Id)
{
	SightCallbacks.erase( eastl::remove_if(SightCallbacks.begin(), SightCallbacks.end(),
			[Id](const CallbackWrapperSight& CWS) { return CWS.Id == Id; }), SightCallbacks.end() );
}

uint64_t PerceptionComponent::AddHearingCallback(HearingCallback Callback)
{
	CallbackWrapperHearing CWH { NextCallbackId++, Callback };
	HearingCallbacks.push_back(CWH);

	return CWH.Id;
}

void PerceptionComponent::RemoveHearingCallback(uint64_t Id)
{
	HearingCallbacks.erase( eastl::remove_if(HearingCallbacks.begin(), HearingCallbacks.end(),
			[Id](const CallbackWrapperHearing& CWH) { return CWH.Id == Id; }), HearingCallbacks.end() );
}

uint64_t PerceptionComponent::AddDamageCallback(DamageCallback Callback)
{
	CallbackWrapperDamage CWD { NextCallbackId++, Callback };
	DamageCallbacks.push_back(CWD);

	return CWD.Id;
}

void PerceptionComponent::RemoveDamageCallback(uint64_t Id)
{
	DamageCallbacks.erase( eastl::remove_if(DamageCallbacks.begin(), DamageCallbacks.end(), 
		[Id](const CallbackWrapperDamage& CWD) { return CWD.Id == Id; }), DamageCallbacks.end() );
}

// ---------------- Actions ----------------
bool PerceptionComponent::MakeNoise(float Loudness) {
	return PerceptionSystem::Get().ReportNoise(this, TeamId, Loudness);
};

void PerceptionComponent::Heard(DXSM::Vector3& Location, float Loudness)
{
	for (auto& CWH : HearingCallbacks)
	{
		if (CWH.Callback)
			CWH.Callback(Location, Loudness);
	}
}

void PerceptionComponent::DealDamage(PerceptionComponent* Instigator, float DamageAmount)
{
	if (!Instigator)
	{
#ifdef _DEBUG
		std::cerr << "[Warning] Cannot deal damage: Instigator is null.\n";
#endif
		return;
	}

	for (auto& CWD : DamageCallbacks)
	{
		if (CWD.Callback)
			CWD.Callback(Instigator->GetOwnerId(), DamageAmount);
	}
};