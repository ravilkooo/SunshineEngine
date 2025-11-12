#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace DirectX::SimpleMath {
    void to_json(json& j, const Vector3& v) {
        j = json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
    }
    void from_json(const json& j, Vector3& v) {
        v.x = j.at("x").get<float>();
        v.y = j.at("y").get<float>();
        v.z = j.at("z").get<float>();
    }
}