#include "AI/Perception/PerceptionComponent.h"

// AI
#include "AI/Perception/PerceptionSystem.h"

// C++
#include <iostream>

// Lua
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

// Json
#include <Serialization/DXSMSerialization.h>



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- SIGHT
// ------------------------------------------------------------------------------------------------------

PerceptionComponent::~PerceptionComponent()
{
	if (TeamId != UINT32_MAX)
	{
		PerceptionSystem::Get().RemoveFromTeam(this);
	}
}

bool PerceptionComponent::SetSight(float NewSightRadius, float NewLoseRadius, float NewFieldOfView, DXSM::Vector3 NewEyesOffset, bool NewCanSeeThroughObjects)
{
	if (NewSightRadius < 0.0f || NewLoseRadius < 0.0f)
	{
		//std::cerr << "[Warning] PerceptionComponent::SetSight: Negative sight values are invalid. SightRadius and LoseRadius must be >= 0.\n";
		return false;
	}

	if (NewLoseRadius < NewSightRadius)
	{
		//std::cerr << "[Warning] PerceptionComponent::SetSight: LoseRadius cannot be smaller than SightRadius.\n";
		return false;
	}

	if (NewFieldOfView > 360.0f || NewFieldOfView < 0.0f)
	{
		//std::cerr << "[Warning] PerceptionComponent::SetSight: FieldOfView must be between 0 and 180 degrees.\n";
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
		//std::cerr << "[Warning] PerceptionComponent::SetSightRadius: SightRadius cannot be negative.\n";
		return false;
	}

	if (NewSightRaduis > LoseRadius)
	{
		//std::cerr << "[Warning] PerceptionComponent::SetSightRadius: SightRadius is bigger than current LoseRadius. LoseRadius automatically adjusted.\n";
		
		LoseRadius = NewSightRaduis;
	}

	SightRadius = NewSightRaduis;

	return true;
}

bool PerceptionComponent::SetLoseRadius(float NewLoseRadius)
{
	if (NewLoseRadius < 0.0f)
	{
		//std::cerr << "[Warning] PerceptionComponent::SetLoseRadius: LoseRadius cannot be negative.\n";
		return false;
	}

	if (NewLoseRadius < SightRadius)
	{
		//std::cerr << "[Warning] PerceptionComponent::SetLoseRadius: LoseRadius cannot be smaller than SightRadius.\n";
		return false;
	}

	LoseRadius = NewLoseRadius;

	return true;
}

bool PerceptionComponent::SetFieldOfView(float NewFieldOfView)
{
	if (NewFieldOfView > 360.0f || NewFieldOfView < 0.0f)
	{
		//std::cerr << "[Warning] PerceptionComponent::SetFieldOfView: FieldOfView must be between 0 and 360 degrees.\n";
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
		//std::cerr << "[Warning] PerceptionComponent::SetHearing: HearingRadius cannot be negative.\n";
		return false;
	}

	if (NewThreshold < 0.0f)
	{
		//std::cerr << "[Warning] PerceptionComponent::SetHearing: Threshold cannot be negative.\n";
		return false;
	}

	if (NewSensitivity < 0.0f)
	{
		//std::cerr << "[Warning] PerceptionComponent::SetHearing: Sensitivity cannot be negative.\n";
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
		//std::cerr << "[Warning] PerceptionComponent::SetHearingRange: HearingRadius cannot be negative.\n";
		return false;
	}

	HearingRadius = NewHearingRadius;

	return true;
}

bool PerceptionComponent::SetThreshold(float NewThreshold)
{
	if (NewThreshold < 0)
	{
		//std::cerr << "[Warning] PerceptionComponent::SetThreshold: Threshold cannot be negative.\n";
		return false;
	}

	Threshold = NewThreshold;

	return true;
}

bool PerceptionComponent::SetSensitivity(float NewSensitivity)
{
	if (NewSensitivity < 0)
	{
		//std::cerr << "[Warning] PerceptionComponent::SetSensitivity: Sensitivity cannot be negative.\n";
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

void PerceptionComponent::ChangeInSight(SE::UUID GOID, bool NewCondition, float InYaw)
{
	if (NewCondition)
	{
		if (std::find(GOCanSee.begin(), GOCanSee.end(), GOID) == GOCanSee.end())
		{
			GOCanSee.push_back(GOID);
		}
	}
	else
	{
		GOCanSee.erase(std::remove(GOCanSee.begin(), GOCanSee.end(), GOID), GOCanSee.end());
	}

	for (auto& Pair : SightCallbacks)
	{
		sol::function& CB = Pair.second;

		if (CB.valid()) [[likely]]
		{
			CB(GOID.GetHilo(), NewCondition, InYaw);
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
			CB(GOID.GetHilo(), Loudness);
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
			CB(Instigator->GetOwnerID().GetHilo(), DamageAmount);
		}
	}
};

void PerceptionComponent::FromJson(const json& j)
{
	if (j.contains("CanSee")) CanSee = j.at("CanSee").get<bool>();
	if (j.contains("EyesOffset")) EyesOffset = j.at("EyesOffset");
	if (j.contains("SightRadius")) SightRadius = j.at("SightRadius").get<float>();
	if (j.contains("LoseRadius")) LoseRadius = j.at("LoseRadius").get<float>();
	if (j.contains("CanSeeThroughObjects")) CanSeeThroughObjects = j.at("CanSeeThroughObjects").get<bool>();
	if (j.contains("FieldOfView")) FieldOfView = j.at("FieldOfView").get<float>();
	if (j.contains("CanHear")) CanHear = j.at("CanHear").get<bool>();
	if (j.contains("HearingRadius")) HearingRadius = j.at("HearingRadius").get<float>();
	if (j.contains("Threshold")) Threshold = j.at("Threshold").get<float>();
	if (j.contains("Sensitivity")) Sensitivity = j.at("Sensitivity").get<float>();
}


// ------------------------------------------------------------------------------------------------------
// ---------------------------------- INFO
// ------------------------------------------------------------------------------------------------------

void PerceptionComponent_Info::SetSightRadius(float NewSightRaduis)
{
	if (NewSightRaduis < 0.0f)
	{
		return;
	}

	if (NewSightRaduis > LoseRadius)
	{
		LoseRadius = NewSightRaduis;

		return;
	}

	SightRadius = NewSightRaduis;
}

void PerceptionComponent_Info::SetLoseRadius(float NewLoseRadius)
{
	if (NewLoseRadius < 0.0f)
	{
		return;
	}

	if (NewLoseRadius < SightRadius)
	{
		LoseRadius = SightRadius;
		return;
	}

	LoseRadius = NewLoseRadius;
}

void PerceptionComponent_Info::SetFieldOfView(float NewFieldOfView)
{
	if (NewFieldOfView > 360.0f || NewFieldOfView < 0.0f)
	{
		return;
	}

	FieldOfView = NewFieldOfView;
}

void PerceptionComponent_Info::SetHearingRadius(float NewHearingRadius)
{
	if (NewHearingRadius < 0)
	{
		return;
	}

	HearingRadius = NewHearingRadius;
}

void PerceptionComponent_Info::SetThreshold(float NewThreshold)
{
	if (NewThreshold < 0)
	{
		return;
	}

	Threshold = NewThreshold;
}

void PerceptionComponent_Info::SetSensitivity(float NewSensitivity)
{
	if (NewSensitivity < 0)
	{
		return;
	}

	Sensitivity = NewSensitivity;
}

json PerceptionComponent_Info::ToJson() const
{
	json j;

	j = nlohmann::json
	{
		{"CanSee",           CanSee},
		{"EyesOffset",       EyesOffset},
		{"SightRadius",      SightRadius},
		{"LoseRadius",       LoseRadius},
		{"FieldOfView",      FieldOfView},
		{"CanSeeThroughObjects",      CanSeeThroughObjects},
		{"CanHear",          CanHear},
		{"HearingRadius",    HearingRadius},
		{"Threshold",        Threshold},
		{"Sensitivity",      Sensitivity},
	};

	return j;
}

void PerceptionComponent_Info::FromJson(const json& j)
{
	if (j.contains("CanSee")) CanSee = j.at("CanSee").get<bool>();
	if (j.contains("EyesOffset")) EyesOffset = j.at("EyesOffset");
	if (j.contains("SightRadius")) SightRadius = j.at("SightRadius").get<float>();
	if (j.contains("LoseRadius")) LoseRadius = j.at("LoseRadius").get<float>();
	if (j.contains("FieldOfView")) FieldOfView = j.at("FieldOfView").get<float>();
	if (j.contains("CanSeeThroughObjects")) CanSeeThroughObjects = j.at("CanSeeThroughObjects").get<bool>();
	if (j.contains("CanHear")) CanHear = j.at("CanHear").get<bool>();
	if (j.contains("HearingRadius")) HearingRadius = j.at("HearingRadius").get<float>();
	if (j.contains("Threshold")) Threshold = j.at("Threshold").get<float>();
	if (j.contains("Sensitivity")) Sensitivity = j.at("Sensitivity").get<float>();
}



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- LUA
// ------------------------------------------------------------------------------------------------------

#define ADD_METHOD(k, fn) k, fn

LUA_REGISTER_COMPONENT(
	PerceptionComponent,
	"PerceptionComponent",
	/* no fields */,
	PERCEPTIONCOMPONENT_LUA_METHODS_APPLY(ADD_METHOD),
	"getPerception"
)
