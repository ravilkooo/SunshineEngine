#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace DirectX {
    namespace SimpleMath {
        inline void to_json(json& j, const Vector3& v) {
            j = json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
        }
        inline void from_json(const json& j, Vector3& v) {
            v.x = j.at("x").get<float>();
            v.y = j.at("y").get<float>();
            v.z = j.at("z").get<float>();
        }
        inline void to_json(json& j, const Vector2& v) {
            j = json{ {"x", v.x}, {"y", v.y} };
        }
        inline void from_json(const json& j, Vector2& v) {
            v.x = j.at("x").get<float>();
            v.y = j.at("y").get<float>();
        }
    }
}

/*
json ToJson(DXSM::Vector3 v) {
    return json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

DXSM::Vector3 FromJson(const json& j) {
    DXSM::Vector3 v;
    v.x = j.at("x").get<float>();
    v.y = j.at("y").get<float>();
    v.z = j.at("z").get<float>();
    return v;
}
*/