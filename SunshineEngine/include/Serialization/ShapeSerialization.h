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
	obj.NumSubdivisions = std::min<uint32_t>(std::max<uint32_t>(obj.NumSubdivisions, 0), 5);
}

inline void to_json(json& j, const CylinderShapeData& obj) {
	j = json{
		{"Radius", obj.Radius},
		{"Height", obj.Height},
		{"SliceCount", obj.SliceCount}
	};
}

inline void from_json(const json& j, CylinderShapeData& obj) {
	j.at("Radius").get_to(obj.Radius);
	j.at("Height").get_to(obj.Height);
	j.at("SliceCount").get_to(obj.SliceCount);
}
