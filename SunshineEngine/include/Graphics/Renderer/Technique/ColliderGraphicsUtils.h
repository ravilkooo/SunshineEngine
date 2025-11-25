#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;


namespace SE_G {

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
            TaperedCapsuleColliderSettings asTaperedCapsule;
        };
    };

    /*
    struct CapsuleColliderSettings : public TaperedCapsuleColliderSettings {
        // m_upperRadius = m_lowerRadius
    };
    */
}