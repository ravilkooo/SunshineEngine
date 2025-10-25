#include "PerceptionComponent.h"

#include <iostream>
#include "PerceptionSystem.h"


void PerceptionComponent::SetSightStruct(SightStruct NewSightSettings)
{
	if (NewSightSettings.SightRange < 0)
	{
		std::cerr << "[Warning]\n";
		return;
	}

	SightSettings = NewSightSettings;
}

void PerceptionComponent::ChangeSightRange(float NewSightRange)
{
	if (NewSightRange < 0) 
	{
		std::cerr << "[Warning]\n";
		return;
	}

	SightSettings.SightRange = NewSightRange;
}

void PerceptionComponent::SetHearingStruct(HearingStruct NewHearingSettings)
{
	if (NewHearingSettings.HearingRange < 0)
	{
		std::cerr << "[Warning]\n";
		return;
	}

	HearingSettings = NewHearingSettings;
}

void PerceptionComponent::ChangeHearingRange(float NewHearingRange)
{
	if (NewHearingRange < 0) 
	{
		std::cerr << "[Warning]\n";
		return;
	}

	HearingSettings.HearingRange = NewHearingRange;
}
