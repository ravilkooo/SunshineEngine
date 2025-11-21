#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <SimpleMath.h>
#include <EASTL/string.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

#include <Physics/PhysicsEnums.h>


namespace DXSM = DirectX::SimpleMath;

namespace SE {
    typedef eastl::string CollisionLayer;
    typedef eastl::string CollisionGroup;

    namespace Layers
    {
        static constexpr JPH::ObjectLayer NON_MOVING = 0;
        static constexpr JPH::ObjectLayer MOVING = 1;
        static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
    };

    namespace BroadPhaseLayers
    {
        static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
        static constexpr JPH::BroadPhaseLayer MOVING(1);
        static constexpr UINT NUM_LAYERS(2);
    };

    struct ColliderVertex {
        DXSM::Vector3 position = { 0.0f, 0.0f, 0.0f };
    };

    struct ColliderBufferOffset {
        UINT vertexStart;
        UINT vertexBufferSize;

        UINT indexStart;
        UINT indexBufferSize;
    };

    // Local transform for collider transforms if needed
    struct ColliderTransforms {
        DXSM::Vector3 m_offset = { 0, 0, 0 };
        DXSM::Vector3 m_rotation = { 0, 0, 0 }; // Pitch (x-axis), Yaw (y-axis), Roll (z-axis)
        DXSM::Vector3 m_scale = { 1, 1, 1 };
    };

    struct ColliderTransformCB {
        DXSM::Matrix mat = DXSM::Matrix::Identity;
    };

    // Params (maximum size = float x8)
    struct RowParams {
        float param0 = 1.0f; float param1 = 1.0f; float param2 = 1.0f; float param3 = 1.0f;
        float param4 = 1.0f; float param5 = 1.0f; float param6 = 1.0f; float param7 = 1.0f;
    };

    struct BoxColliderSettings
    {
        DXSM::Vector3 m_size = DXSM::Vector3::One;
    };

    struct SphereColliderSettings
    {
        float m_radius = 1.0f;
    };

    struct CapsuleColliderSettings {
        float m_height = 1.0f;
        float m_radius = 1.0f;
    };

    struct TaperedCapsuleColliderSettings
    {
        float m_height = 1.0f;
        float m_upperRadius = 1.0f;
        float m_lowerRadius = 1.0f;
    };

    struct ColliderSettings
    {
        union {
            RowParams asRowParams; // For setting maximum size

            BoxColliderSettings asBox;
            SphereColliderSettings asSphere;
            CapsuleColliderSettings asCapsule;
            TaperedCapsuleColliderSettings asTaperedCapsule;
        };
    };

    class ColliderData
    {
    public:
        ColliderTransforms m_transformsData;
        ColliderTransformCB m_transformMat;
        ColliderShapeType m_shapeType;
        ColliderSettings m_settings;

        ColliderData() : m_settings({})
        {
            m_transformsData = {
                { 0, 0, 0 },
                { 0, 0, 0 },
                { 1, 1, 1 }
            };

            m_transformMat = { DXSM::Matrix::Identity };

            m_shapeType = ColliderShapeType::Box;

            m_settings.asBox = { DXSM::Vector3::One };
        }

        ColliderData(const ColliderData& cd) : m_settings({}) {
            m_transformsData = cd.m_transformsData;
            m_transformMat = cd.m_transformMat;
            m_shapeType = cd.m_shapeType;
            m_settings = cd.m_settings;
        }

        ColliderData(ColliderData&& cd) noexcept : m_settings({}) {
            m_transformsData = eastl::move(cd.m_transformsData);
            m_transformMat = eastl::move(cd.m_transformMat);
            m_shapeType = eastl::move(cd.m_shapeType);
            m_settings = eastl::move(cd.m_settings);
        }

        ColliderData(ColliderShapeType shapeType) :
            m_settings({}), m_shapeType(shapeType)
        {
            m_transformsData = {
                { 0, 0, 0 },
                { 0, 0, 0 },
                { 1, 1, 1 }
            };

            m_transformMat = { DXSM::Matrix::Identity };


            switch (m_shapeType)
            {
            case SE::ColliderShapeType::Box:
                m_settings.asBox = { { 1.0f, 1.0f, 1.0f } };
                break;

            case SE::ColliderShapeType::Sphere:
                m_settings.asSphere = { 1.0f };
                break;

            case SE::ColliderShapeType::Capsule:
                m_settings.asCapsule = { 1.0f, 1.0f };
                break;

            case SE::ColliderShapeType::TaperedCapsule:
                m_settings.asTaperedCapsule = { 1.0f, 1.0f, 1.0f };
                break;

            case SE::ColliderShapeType::Mesh:
            default:
                m_settings.asBox = { { 1.0f, 1.0f, 1.0f } };
                break;
            }
        }


        ColliderData& operator=(const ColliderData& cd) {
            if (this != &cd) {
                m_transformsData = cd.m_transformsData;
                m_transformMat = cd.m_transformMat;
                m_shapeType = cd.m_shapeType;
                m_settings = cd.m_settings;
            }
            return *this;
        }

        ColliderData& operator=(ColliderData&& cd) noexcept {
            if (this != &cd) {
                m_transformsData = eastl::move(cd.m_transformsData);
                m_transformMat = eastl::move(cd.m_transformMat);
                m_shapeType = eastl::move(cd.m_shapeType);
                m_settings = eastl::move(cd.m_settings);
            }
            return *this;
        }

        ColliderTransforms GetTransformData()
        {
            return m_transformsData;
        }

        void SetTransformData(ColliderTransforms transform)
        {
            m_transformsData = transform;

            m_transformMat =
            {
                DXSM::Matrix::CreateTranslation(m_transformsData.m_offset) *
                DXSM::Matrix::CreateFromYawPitchRoll(m_transformsData.m_rotation.y,
                    m_transformsData.m_rotation.x, m_transformsData.m_rotation.z) *
                DXSM::Matrix::CreateScale(m_transformsData.m_scale)
            };
        }

        ColliderShapeType GetShapeType()
        {
            return m_shapeType;
        }

        void SetShapeType(ColliderShapeType shapeType)
        {
            m_shapeType = shapeType;

            switch (m_shapeType)
            {
            case ColliderShapeType::Box:
                m_settings.asBox = { { 1.0f, 1.0f, 1.0f } };
                break;

            case ColliderShapeType::Sphere:
                m_settings.asSphere = { 1.0f };
                break;

            case ColliderShapeType::Capsule:
                m_settings.asCapsule = { 1.0f, 1.0f };
                break;

            case ColliderShapeType::TaperedCapsule:
                m_settings.asTaperedCapsule = { 1.0f, 1.0f, 1.0f };
                break;

            case ColliderShapeType::Mesh:
            default:
                break;
            }
        }

        ColliderSettings GetColliderSettings()
        {
            return m_settings;
        }

        void SetColliderSettings(ColliderSettings settings)
        {
            m_settings = settings;
        }

        // Serialize this ColliderData to JSON
        nlohmann::json ToJson() const
        {
            nlohmann::json j;

            j["transforms"]["offset"] = { m_transformsData.m_offset.x, m_transformsData.m_offset.y, m_transformsData.m_offset.z };
            j["transforms"]["rotation"] = { m_transformsData.m_rotation.x, m_transformsData.m_rotation.y, m_transformsData.m_rotation.z };
            j["transforms"]["scale"] = { m_transformsData.m_scale.x, m_transformsData.m_scale.y, m_transformsData.m_scale.z };

            j["shapeType"] = m_shapeType;

            switch (m_shapeType)
            {
            case ColliderShapeType::Box:
                j["settings"]["box"]["size"] = { m_settings.asBox.m_size.x, m_settings.asBox.m_size.y, m_settings.asBox.m_size.z };
                break;
            case ColliderShapeType::Sphere:
                j["settings"]["sphere"]["radius"] = m_settings.asSphere.m_radius;
                break;
            case ColliderShapeType::Capsule:
                j["settings"]["capsule"]["height"] = m_settings.asCapsule.m_height;
                j["settings"]["capsule"]["radius"] = m_settings.asCapsule.m_radius;
                break;
            case ColliderShapeType::TaperedCapsule:
                j["settings"]["taperedCapsule"]["height"] = m_settings.asTaperedCapsule.m_height;
                j["settings"]["taperedCapsule"]["upperRadius"] = m_settings.asTaperedCapsule.m_upperRadius;
                j["settings"]["taperedCapsule"]["lowerRadius"] = m_settings.asTaperedCapsule.m_lowerRadius;
                break;
            case ColliderShapeType::Mesh:
            default:
                // No additional data for mesh/default
                break;
            }

            return j;
        }

        // Deserialize from JSON into this ColliderData
        void FromJson(const nlohmann::json& j)
        {
            if (j.contains("transforms")) {
                auto t = j["transforms"];
                if (t.contains("offset") && t["offset"].is_array() && t["offset"].size() >= 3) {
                    m_transformsData.m_offset.x = t["offset"][0].get<float>();
                    m_transformsData.m_offset.y = t["offset"][1].get<float>();
                    m_transformsData.m_offset.z = t["offset"][2].get<float>();
                }
                if (t.contains("rotation") && t["rotation"].is_array() && t["rotation"].size() >= 3) {
                    m_transformsData.m_rotation.x = t["rotation"][0].get<float>();
                    m_transformsData.m_rotation.y = t["rotation"][1].get<float>();
                    m_transformsData.m_rotation.z = t["rotation"][2].get<float>();
                }
                if (t.contains("scale") && t["scale"].is_array() && t["scale"].size() >= 3) {
                    m_transformsData.m_scale.x = t["scale"][0].get<float>();
                    m_transformsData.m_scale.y = t["scale"][1].get<float>();
                    m_transformsData.m_scale.z = t["scale"][2].get<float>();
                }
            }

            if (j.contains("shapeType")) {
                j.at("shapeType").get_to(m_shapeType);
            }

            // Read settings depending on shape
            if (j.contains("settings")) {
                auto s = j["settings"];
                switch (m_shapeType)
                {
                case ColliderShapeType::Box:
                    if (s.contains("box") && s["box"].contains("size") && s["box"]["size"].is_array()) {
                        auto a = s["box"]["size"];
                        m_settings.asBox.m_size.x = a[0].get<float>();
                        m_settings.asBox.m_size.y = a[1].get<float>();
                        m_settings.asBox.m_size.z = a[2].get<float>();
                    }
                    break;
                case ColliderShapeType::Sphere:
                    if (s.contains("sphere") && s["sphere"].contains("radius")) {
                        m_settings.asSphere.m_radius = s["sphere"]["radius"].get<float>();
                    }
                    break;
                case ColliderShapeType::Capsule:
                    if (s.contains("capsule")) {
                        if (s["capsule"].contains("height"))
                            m_settings.asCapsule.m_height = s["capsule"]["height"].get<float>();
                        if (s["capsule"].contains("radius"))
                            m_settings.asCapsule.m_radius = s["capsule"]["radius"].get<float>();
                    }
                    break;
                case ColliderShapeType::TaperedCapsule:
                    if (s.contains("taperedCapsule")) {
                        if (s["taperedCapsule"].contains("height"))
                            m_settings.asTaperedCapsule.m_height = s["taperedCapsule"]["height"].get<float>();
                        if (s["taperedCapsule"].contains("upperRadius"))
                            m_settings.asTaperedCapsule.m_upperRadius = s["taperedCapsule"]["upperRadius"].get<float>();
                        if (s["taperedCapsule"].contains("lowerRadius"))
                            m_settings.asTaperedCapsule.m_lowerRadius = s["taperedCapsule"]["lowerRadius"].get<float>();
                    }
                    break;
                case ColliderShapeType::Mesh:
                default:
                    break;
                }
            }

            // Recompute transform matrix from transforms
            SetTransformData(m_transformsData);
        }
    };
}