#pragma once

#include <GameObject/Shapes/ShapeData.h>
#include <Serialization/DXSMSerialization.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Serialization for simple shape descriptions used by the editor and factories

inline void to_json(json& j, const BoxShapeData& obj) {
	j = json{
		{"Size", obj.Size}
	};
}

inline void from_json(const json& j, BoxShapeData& obj) {
	j.at("Size").get_to(obj.Size);
}

inline void to_json(json& j, const SphereShapeData& obj) {
	j = json{
		{"Size", obj.Size},
		{"SliceCount", obj.SliceCount},
		{"StackCount", obj.StackCount}
	};
}

inline void from_json(const json& j, SphereShapeData& obj) {
	j.at("Size").get_to(obj.Size);
	j.at("SliceCount").get_to(obj.SliceCount);
	j.at("StackCount").get_to(obj.StackCount);
}

inline void to_json(json& j, const GeosphereShapeData& obj) {
	j = json{
		{"Size", obj.Size},
		{"NumSubdivisions", obj.NumSubdivisions}
	};
}

inline void from_json(const json& j, GeosphereShapeData& obj) {
	j.at("Size").get_to(obj.Size);
	j.at("NumSubdivisions").get_to(obj.NumSubdivisions);
}

