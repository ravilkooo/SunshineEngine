#include "AI/Perception/PerceptionComponent.h"

#include <iostream>
#include "AI/Perception/PerceptionSystem.h"


// ---------------- Sight ----------------
void PerceptionComponent::SetSightStruct(SightStruct NewSightSettings)
{
	if (NewSightSettings.SightRadius < 0 || NewSightSettings.LoseRadius < 0)
	{
		std::cerr << "[Warning] Negative sight values are invalid. SightRadius and LoseRadius must be >= 0.\n";
		return;
	}

	if (NewSightSettings.LoseRadius < NewSightSettings.SightRadius)
	{
		std::cerr << "[Warning] LoseRadius cannot be smaller than SightRadius.\n";
		return;
	}

	if (NewSightSettings.FieldOfView > 180 || NewSightSettings.FieldOfView < 0)
	{
		std::cerr << "[Warning] FieldOfView must be between 0 and 180 degrees.\n";
		return;
	}

	SightSettings = NewSightSettings;
}

void PerceptionComponent::ChangeSightRadius(float NewSightRaduis)
{
	if (NewSightRaduis < 0) 
	{
		std::cerr << "[Warning] SightRadius cannot be negative.\n";
		return;
	}

	if (NewSightRaduis < SightSettings.LoseRadius)
	{
		std::cerr << "[Warning] SightRadius is smaller than current LoseRadius. LoseRadius automatically adjusted.\n";
		
		SightSettings.LoseRadius = NewSightRaduis;
	}

	SightSettings.SightRadius = NewSightRaduis;
}

void PerceptionComponent::ChangeLoseRadius(float NewLoseRadius)
{
	if (NewLoseRadius < SightSettings.SightRadius)
	{
		std::cerr << "[Warning] LoseRadius cannot be smaller than SightRadius.\n";
		return;
	}

	SightSettings.LoseRadius = NewLoseRadius;
}

void PerceptionComponent::ChangeFieldOfView(float NewFieldOfView)
{
	if (NewFieldOfView > 180 || NewFieldOfView < 0)
	{
		std::cerr << "[Warning] FieldOfView must be between 0 and 180 degrees.\n";
		return;
	}

	SightSettings.FieldOfView = NewFieldOfView;
}

// ---------------- Hearing ----------------
void PerceptionComponent::SetHearingStruct(HearingStruct NewHearingSettings)
{
	if (NewHearingSettings.HearingRadius < 0)
	{
		std::cerr << "[Warning] HearingRadius cannot be negative.\n";
		return;
	}

	if (NewHearingSettings.Threshold < 0)
	{
		std::cerr << "[Warning] Threshold cannot be negative.\n";
		return;
	}

	if (NewHearingSettings.Sensitivity < 0)
	{
		std::cerr << "[Warning] Sensitivity cannot be negative.\n";
		return;
	}

	HearingSettings = NewHearingSettings;
}

void PerceptionComponent::ChangeHearingRange(float NewHearingRange)
{
	if (NewHearingRange < 0) 
	{
		std::cerr << "[Warning] HearingRadius cannot be negative.\n";
		return;
	}

	HearingSettings.HearingRadius = NewHearingRange;
}

void PerceptionComponent::ChangeThreshold(float NewThreshold)
{
	if (NewThreshold < 0)
	{
		std::cerr << "[Warning] Threshold cannot be negative.\n";
		return;
	}

	HearingSettings.Threshold = NewThreshold;
}

void PerceptionComponent::ChangeSensitivity(float NewSensitivity)
{
	if (NewSensitivity < 0)
	{
		std::cerr << "[Warning] Sensitivity cannot be negative.\n";
		return;
	}

	HearingSettings.Sensitivity = NewSensitivity;
}

// ---------------- Listeners ----------------
uint64_t PerceptionComponent::AddSightListener(SightCallback Callback)
{
	ListenerWrapperSight LW { NextListenerId++, Callback };
	SightListeners.push_back(LW);

	return LW.Id;
}

void PerceptionComponent::RemoveSightListener(uint64_t Id)
{
	SightListeners.erase( eastl::remove_if(SightListeners.begin(), SightListeners.end(),
			[Id](const ListenerWrapperSight& LWS) { return LWS.Id == Id; }), SightListeners.end() );
}

uint64_t PerceptionComponent::AddHearingListener(HearingCallback Callback)
{
	ListenerWrapperHearing LW { NextListenerId++, Callback };
	HearingListeners.push_back(LW);

	return LW.Id;
}

void PerceptionComponent::RemoveHearingListener(uint64_t Id)
{
	HearingListeners.erase( eastl::remove_if(HearingListeners.begin(), HearingListeners.end(),
			[Id](const ListenerWrapperHearing& LWH) { return LWH.Id == Id; }), HearingListeners.end() );
}

uint64_t PerceptionComponent::AddDamageListener(DamageCallback Callback)
{
	ListenerWrapperDamage LW { NextListenerId++, Callback };
	DamageListeners.push_back(LW);

	return LW.Id;
}

void PerceptionComponent::RemoveDamageListener(uint64_t Id)
{
	DamageListeners.erase( eastl::remove_if(DamageListeners.begin(), DamageListeners.end(), 
		[Id](const ListenerWrapperDamage& LWD) { return LWD.Id == Id; }), DamageListeners.end() );
}

// ---------------- Actions ----------------
void PerceptionComponent::MakeNoise(float Loudness) {
	PerceptionSystem::Get().ReportNoise(this, TeamId, Loudness);
};

void PerceptionComponent::Heard(bool& Location, float Loudness)
{
	for (auto& LW : HearingListeners)
	{
		if (LW.Func)
			LW.Func(Location, Loudness);
	}
}

void PerceptionComponent::DealDamage(PerceptionComponent* Instigator, float DamageAmount)
{
	if (!Instigator)
	{
		std::cerr << "[Warning] Cannot deal damage: Instigator is null.\n";
		return;
	}

	for (auto& LW : DamageListeners)
	{
		if (LW.Func)
			LW.Func(Instigator->GetOwner(), DamageAmount);
	}
};