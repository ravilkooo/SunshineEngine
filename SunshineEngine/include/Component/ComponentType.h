#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace SE {
	enum class ComponentType {
		TRANSFORM,
		RENDER,
		LUA,
		PHYSICS,
		PERCEPTION,
		BEHAVIOR,
		MESH
	};
}


NLOHMANN_JSON_SERIALIZE_ENUM(SE::ComponentType, {
	{SE::ComponentType::TRANSFORM, "TRANSFORM"},
	{SE::ComponentType::RENDER, "RENDER"},
	{SE::ComponentType::LUA,   "LUA"},
	{SE::ComponentType::PHYSICS,   "PHYSICS"},
	{SE::ComponentType::PERCEPTION,   "PERCEPTION"},
	{SE::ComponentType::BEHAVIOR,   "BEHAVIOR"},
	{SE::ComponentType::MESH,   "MESH"},
})