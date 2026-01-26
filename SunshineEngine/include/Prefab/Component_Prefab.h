#pragma once

#include <SimpleMath.h>

#include <Utils/AssetPath.h>
#include <GameObject/ObjectGroupType.h>
#include <Component/ComponentType.h>

#include <Graphics/Bindable/Sampler.h>

#include <Physics/CollisionUtils.h>

namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

class Component_Prefab
{
public:
    SE::ComponentType m_type;
};

class Transform_Prefab : public Component_Prefab
{
public:
    // Transform
    DXSM::Vector3 m_position = { 0, 0, 0 };
    DXSM::Vector3 m_rotation = { 0, 0, 0 }; // Pitch (x-axis), Yaw (y-axis), Roll (z-axis)
    DXSM::Vector3 m_scaleFactor = { 1, 1, 1 };

    // Local Transform
    DXSM::Vector3 m_localPosition = { 0, 0, 0 };
    DXSM::Vector3 m_localRotation = { 0, 0, 0 }; // Pitch (x-axis), Yaw (y-axis), Roll (z-axis)
    DXSM::Vector3 m_localScaleFactor = { 1, 1, 1 };

    Transform_Prefab()
    {
        m_type = SE::ComponentType::TRANSFORM;
    }
};

class Mesh_Prefab : public Component_Prefab
{
public:
    AssetPath m_meshPath = AssetPath(L"Box_repeat");
    AssetPath m_texPath = AssetPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);;
    SE_G::Bind::Sampler m_textureSampler;

    Mesh_Prefab()
    {
        m_type = SE::ComponentType::MESH;
    }
};

class Physics_Prefab : public Component_Prefab
{
public:
    SE::ColliderData m_colliderData;

    SE::PhysicsMotionType m_motion = SE::PhysicsMotionType::Static;
    SE::PhysicsActivation m_activation = SE::PhysicsActivation::DontActivate;
    SE::CollisionLayer m_collisionLayer = "MOVING";

    Physics_Prefab()
    {
        m_type = SE::ComponentType::PHYSICS;
    }
};

class Trigger_Prefab : public Component_Prefab
{
public:
    SE::ColliderData* m_colliderData;

    Trigger_Prefab()
    {
        m_type = SE::ComponentType::TRIGGER;
    }
};

class Perception_Prefab : public Component_Prefab
{
public:
    bool CanSee = false;
    DXSM::Vector3 EyesOffset = DXSM::Vector3::Zero;
    float SightRadius = 0.0f;
    float LoseRadius = 0.0f;
    float FieldOfView = 90.0f;
    bool CanSeeThroughObjects = false;
    bool CanHear = false;
    float HearingRadius = 0.0f;
    float Threshold = 0.0f;
    float Sensitivity = 1.0f;

    Perception_Prefab()
    {
        m_type = SE::ComponentType::PERCEPTION;
    }
};

class Behavior_Prefab : public Component_Prefab
{
public:
    bool m_isEnabled = false;

    Behavior_Prefab()
    {
        m_type = SE::ComponentType::BEHAVIOR;
    }
};

class Lua_Prefab : public Component_Prefab
{
public:
    AssetPath m_luaPath;

    Lua_Prefab()
    {
        m_type = SE::ComponentType::LUA;
    }
};

class Emitter_Prefab : public Component_Prefab
{
public:
    DXSM::Vector3 position;
    
    DXSM::Vector3 colorStart;
    DXSM::Vector3 colorEnd;

    float alphaStart;
    float alphaEnd;

    uint32_t maxSpawn;
    float m_deaultEmissionRate = 0.0f;

    float particlesLifeSpan;
    float particlesBaseSpeed;

    float particlesMass;

    float particleSizeStart;
    float particleSizeEnd;

    float longitudeMin;
    float longitudeMax;

    float latitudeMin;
    float latitudeMax;

    DXSM::Vector3 force;

    AssetPath texPath;

    Emitter_Prefab()
    {
        m_type = SE::ComponentType::PARTICLE_EMITTER;
    }
};
