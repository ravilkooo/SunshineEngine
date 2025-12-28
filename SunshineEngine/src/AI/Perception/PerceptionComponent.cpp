#include "AI/Perception/PerceptionComponent.h"

// AI
#include "AI/Perception/PerceptionSystem.h"

// C++
#include <iostream>

// Lua
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- SIGHT
// ------------------------------------------------------------------------------------------------------

PerceptionComponent::~PerceptionComponent()
{
	PerceptionSystem::Get().RemoveFromTeam(TeamId, this);
}

bool PerceptionComponent::SetSight(float NewSightRadius, float NewLoseRadius, float NewFieldOfView, DXSM::Vector3 NewEyesOffset, bool NewCanSeeThroughObjects)
{
	if (NewSightRadius < 0.0f || NewLoseRadius < 0.0f)
	{
		std::cerr << "[Warning] PerceptionComponent::SetSightStruct: Negative sight values are invalid. SightRadius and LoseRadius must be >= 0.\n";
		return false;
	}

	if (NewLoseRadius < NewSightRadius)
	{
		std::cerr << "[Warning] PerceptionComponent::SetSightStruct: LoseRadius cannot be smaller than SightRadius.\n";
		return false;
	}

	if (NewFieldOfView > 360.0f || NewFieldOfView < 0.0f)
	{
		std::cerr << "[Warning] PerceptionComponent::SetSightStruct: FieldOfView must be between 0 and 180 degrees.\n";
		return false;
	}

	SightRadius = NewSightRadius;
	LoseRadius = NewLoseRadius;
	FieldOfView = NewFieldOfView;
	EyesOffset = NewEyesOffset;
	CanSeeThroughObjects = NewCanSeeThroughObjects;

	return true;
}

bool PerceptionComponent::SetSightRadius(float NewSightRaduis)
{
	if (NewSightRaduis < 0.0f) 
	{
		std::cerr << "[Warning] PerceptionComponent::ChangeSightRadius: SightRadius cannot be negative.\n";
		return false;
	}

	if (NewSightRaduis < LoseRadius)
	{
		std::cerr << "[Warning] PerceptionComponent::ChangeSightRadius: SightRadius is smaller than current LoseRadius. LoseRadius automatically adjusted.\n";
		
		LoseRadius = NewSightRaduis;
	}

	SightRadius = NewSightRaduis;

	return true;
}

bool PerceptionComponent::SetLoseRadius(float NewLoseRadius)
{
	if (NewLoseRadius < 0.0f)
	{
		std::cerr << "[Warning] PerceptionComponent::ChangeLoseRadius: LoseRadius cannot be negative.\n";
		return false;
	}

	if (NewLoseRadius < SightRadius)
	{
		std::cerr << "[Warning] PerceptionComponent::ChangeLoseRadius: LoseRadius cannot be smaller than SightRadius.\n";
		return false;
	}

	LoseRadius = NewLoseRadius;

	return true;
}

bool PerceptionComponent::SetFieldOfView(float NewFieldOfView)
{
	if (NewFieldOfView > 360.0f || NewFieldOfView < 0.0f)
	{
		std::cerr << "[Warning] PerceptionComponent::ChangeFieldOfView: FieldOfView must be between 0 and 360 degrees.\n";
		return false;
	}

	FieldOfView = NewFieldOfView;

	return true;
}




// ------------------------------------------------------------------------------------------------------
// ---------------------------------- HEARING
// ------------------------------------------------------------------------------------------------------

bool PerceptionComponent::SetHearing(float NewHearingRadius, float NewThreshold, float NewSensitivity)
{
	if (NewHearingRadius < 0.0f)
	{
		std::cerr << "[Warning] PerceptionComponent::SetHearingStruct: HearingRadius cannot be negative.\n";
		return false;
	}

	if (NewThreshold < 0.0f)
	{
		std::cerr << "[Warning] PerceptionComponent::SetHearingStruct: Threshold cannot be negative.\n";
		return false;
	}

	if (NewSensitivity < 0.0f)
	{
		std::cerr << "[Warning] PerceptionComponent::SetHearingStruct: Sensitivity cannot be negative.\n";
		return false;
	}

	HearingRadius = NewHearingRadius;
	Threshold = NewThreshold;
	Sensitivity = NewSensitivity;

	return true;
}

bool PerceptionComponent::SetHearingRadius(float NewHearingRadius)
{
	if (NewHearingRadius < 0)
	{
		std::cerr << "[Warning] PerceptionComponent::ChangeHearingRange: HearingRadius cannot be negative.\n";
		return false;
	}

	HearingRadius = NewHearingRadius;

	return true;
}

bool PerceptionComponent::SetThreshold(float NewThreshold)
{
	if (NewThreshold < 0)
	{
		std::cerr << "[Warning] PerceptionComponent::ChangeThreshold: Threshold cannot be negative.\n";
		return false;
	}

	Threshold = NewThreshold;

	return true;
}

bool PerceptionComponent::SetSensitivity(float NewSensitivity)
{
	if (NewSensitivity < 0)
	{
		std::cerr << "[Warning] PerceptionComponent::ChangeSensitivity: Sensitivity cannot be negative.\n";
		return false;
	}

	Sensitivity = NewSensitivity;

	return true;
}




// ------------------------------------------------------------------------------------------------------
// ---------------------------------- LISTENERS
// ------------------------------------------------------------------------------------------------------

uint64_t PerceptionComponent::AddSightCallback(const sol::function& Callback)
{
	const uint64_t id = NextCallbackId++;
	SightCallbacks.emplace(id, Callback);

	return id;
}

uint64_t PerceptionComponent::AddHearingCallback(const sol::function& Callback)
{
	const uint64_t id = NextCallbackId++;
	HearingCallbacks.emplace(id, Callback);

	return id;
}

uint64_t PerceptionComponent::AddDamageCallback(const sol::function& Callback)
{
	const uint64_t id = NextCallbackId++;
	DamageCallbacks.emplace(id, Callback);

	return id;
}




// ------------------------------------------------------------------------------------------------------
// ---------------------------------- EVENTS
// ------------------------------------------------------------------------------------------------------

bool PerceptionComponent::MakeNoise(float Loudness) 
{
	return PerceptionSystem::Get().ReportNoise(this, Loudness);
};

void PerceptionComponent::ChangeInSight(SE::UUID GOID, bool NewCondition)
{
	if (NewCondition)
	{
		GOCanSee.push_back(GOID);
	}
	else
	{
		auto it = eastl::find(GOCanSee.begin(), GOCanSee.end(), GOID);

		if (it != GOCanSee.end())
		{
			GOCanSee.erase(it);
		}
	}

	for (auto& Pair : SightCallbacks)
	{
		sol::function& CB = Pair.second;

		if (CB.valid()) [[likely]]
		{
			CB(GOID, NewCondition);
		}
	}
}

void PerceptionComponent::Heard(SE::UUID GOID, float Loudness)
{
	for (auto& Pair : HearingCallbacks)
	{
		sol::function& CB = Pair.second;

		if (CB.valid()) [[likely]]
		{
			CB(GOID, Loudness);
		}
	}
}

void PerceptionComponent::DealDamage(PerceptionComponent* Instigator, float DamageAmount)
{
	if (!Instigator)
	{
		std::cerr << "[Warning] PerceptionComponent::DealDamage: Instigator is null.\n";
		return;
	}

	for (auto& Pair : DamageCallbacks)
	{
		sol::function& CB = Pair.second;

		if (CB.valid()) [[likely]]
		{
			CB(Instigator->GetOwnerID(), DamageAmount);
		}
	}
};


// ------------------------------------------------------------------------------------------------------
// ---------------------------------- LUA
// ------------------------------------------------------------------------------------------------------

#define ADD_METHOD(k, fn) k, fn

LUA_REGISTER_COMPONENT(
	PerceptionComponent,
	"PerceptionComponent",
	/* no fields */,
	PERCEPTIONCOMPONENT_LUA_METHODS_APPLY(ADD_METHOD),
	"getPerceptionComponent"
)

/*
#define PC_ADD_FIELD(name) #name, &PerceptionComponent::name
#define PERCEPTIONCOMPONENT_FIELD_PAIRS \
    PERCEPTIONCOMPONENT_LUA_FIELDS_APPLY(PC_ADD_FIELD)
#undef PC_ADD_FIELD

#define PC_ADD_METHOD_WITH_LEAD(k, fn) , k, fn
#define PERCEPTIONCOMPONENT_METHOD_PAIRS \
    PERCEPTIONCOMPONENT_LUA_METHODS_APPLY(PC_ADD_METHOD_WITH_LEAD)
#undef PC_ADD_METHOD_WITH_LEAD


LUA_REGISTER_COMPONENT(
    PerceptionComponent,
    "PerceptionComponent",
    PERCEPTIONCOMPONENT_FIELD_PAIRS
    PERCEPTIONCOMPONENT_METHOD_PAIRS,
    "getPerceptionComponent"
)
*/
