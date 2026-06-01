#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

enum class GameObjectGroup {
    Lighting, Shapes, CustomMesh, ParticleEmitter, Other
};

NLOHMANN_JSON_SERIALIZE_ENUM(GameObjectGroup, {
    {GameObjectGroup::Lighting, "Lighting"},
    {GameObjectGroup::Shapes, "Shapes"},
    {GameObjectGroup::CustomMesh, "CustomMesh"},
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
    Box, Sphere, Geosphere, Cylinder, Plane
};

NLOHMANN_JSON_SERIALIZE_ENUM(ShapeObjectType, {
    {ShapeObjectType::Box, "Box"},
    {ShapeObjectType::Sphere, "Sphere"},
    {ShapeObjectType::Geosphere, "Geosphere"},
    {ShapeObjectType::Cylinder, "Cylinder"},
    {ShapeObjectType::Plane, "Plane"},
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