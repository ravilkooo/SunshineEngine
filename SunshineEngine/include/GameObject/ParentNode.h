#pragma once

#include <Utils/UUID.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

template<class T>
class ParentNode
{
public:
	// Or UUID?
	SE::UUID uuid = SE::UUID(0u);
	T* ptr = nullptr;

	// Apply parent transform and PhysComponent should be Kinematic (Jolt)?
	// to-do: change to enum class
	// AttachMode:
	// None, Free, FixedConstraint (Fixed, when both have Physics), FixedGhost (Fixed, when both haven't Physics)
	// ParentKinematicControl (Child has Physics, child controlled by parent)
	// ParentGhostControl (Child hasn't Physics, child controlled by parent)
	bool attached = false;

	// Serialization
	json ToJson() const
	{
		json j;
		if (ptr)
		{
			j["uuid"] = (uint64_t) uuid.m_UUID;
			j["attached"] = attached;
		}
		return j;
	}


	static ParentNode<T> FromJson(const json& j)
	{
		ParentNode<T> pn;

		if (j.contains("uuid"))
			pn.uuid = SE::UUID(j["uuid"].get<uint64_t>());

		if (j.contains("attached"))
			pn.attached = j["attached"];

		return pn;
	}
};
