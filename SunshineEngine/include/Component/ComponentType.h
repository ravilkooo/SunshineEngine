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
		TRIGGER,
		BEHAVIOR,
		MESH,
		CAMERA,
		PARTICLE_EMITTER,
		CHARACTER,
		BOUNCE_PAD,
		CHARACTER_CONTROLLER,
		COUNT
	};
}


NLOHMANN_JSON_SERIALIZE_ENUM(SE::ComponentType, {
	{SE::ComponentType::TRANSFORM, "TRANSFORM"},
	{SE::ComponentType::RENDER, "RENDER"},
	{SE::ComponentType::LUA,   "LUA"},
	{SE::ComponentType::PHYSICS,   "PHYSICS"},
	{SE::ComponentType::PERCEPTION,   "PERCEPTION"},
	{SE::ComponentType::TRIGGER,   "TRIGGER"},
	{SE::ComponentType::BEHAVIOR,   "BEHAVIOR"},
	{SE::ComponentType::MESH,   "MESH"},
	{SE::ComponentType::CAMERA,   "CAMERA"},
	{SE::ComponentType::PARTICLE_EMITTER,   "PARTICLE_EMITTER"},
	{SE::ComponentType::CHARACTER,   "CHARACTER"},
	{SE::ComponentType::CHARACTER_CONTROLLER,   "CHARACTER_CONTROLLER"},
	{SE::ComponentType::BOUNCE_PAD,   "BOUNCE_PAD"},
})