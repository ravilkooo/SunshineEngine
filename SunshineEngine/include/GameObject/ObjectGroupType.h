#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

enum class GameObjectGroup {
    Lighting, Shapes, CustomMesh, Player, ParticleEmitter, Other
};

NLOHMANN_JSON_SERIALIZE_ENUM(GameObjectGroup, {
    {GameObjectGroup::Lighting, "Lighting"},
    {GameObjectGroup::Shapes, "Shapes"},
    {GameObjectGroup::CustomMesh, "CustomMesh"},
    {GameObjectGroup::Player, "Player"},
    {GameObjectGroup::ParticleEmitter, "ParticleEmitter"},
    {GameObjectGroup::Other, "Other"},
    })

enum class LightObjectType {
    PointLight, DirectionalLight, SkyBox, AmbientLight, SpotLight,
};

NLOHMANN_JSON_SERIALIZE_ENUM(LightObjectType, {
    {LightObjectType::PointLight, "PointLight"},
    {LightObjectType::DirectionalLight, "DirectionalLight"},
    {LightObjectType::SkyBox, "SkyBox"},
    {LightObjectType::AmbientLight, "AmbientLight"},
    {LightObjectType::AmbientLight, "SpotLight"},
    })

enum class ShapeObjectType {
    Box, Sphere, Geosphere, Cylinder
};

NLOHMANN_JSON_SERIALIZE_ENUM(ShapeObjectType, {
    {ShapeObjectType::Box, "Box"},
    {ShapeObjectType::Sphere, "Sphere"},
    {ShapeObjectType::Geosphere, "Geosphere"},
    {ShapeObjectType::Cylinder, "Cylinder"},
    })

struct ObjectType {
    union {
        LightObjectType m_asLight;
        ShapeObjectType m_asShape;
    };

    ObjectType() {}

    ObjectType(GameObjectGroup objGroup, const json& j)
    {
        switch (objGroup)
        {
        case GameObjectGroup::Lighting:
            m_asLight = j; // j["m_type"];
            break;
        case GameObjectGroup::Shapes:
            m_asShape = j; // j["m_type"];
            break;
        case GameObjectGroup::CustomMesh:
            break;
        case GameObjectGroup::Other:
            break;
        default:
            break;
        }
    }
};