#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;

namespace SE_G {
    struct AmbientLightData {
        DXSM::Vector3 Ambient;
        float AmbientPad;
    };

    struct DirectionalLightData {
        DXSM::Vector3 Diffuse;
        float DiffusePad;
        DXSM::Vector3 Specular;
        float SpecularPad;
        DXSM::Vector3 Position;
        float pad1;

        DXSM::Vector3 Direction;
        float pad2;
    };

    struct PointLightData {
        DXSM::Vector3 Diffuse;
        float DiffusePad;
        DXSM::Vector3 Specular;
        float SpecularPad;
        DXSM::Vector3 Position;
        float Range;

        DXSM::Vector3 Att;
        float pad;
    };

    struct SpotLightData {
        DXSM::Vector3 Diffuse;
        float DiffusePad;
        DXSM::Vector3 Specular;
        float SpecularPad;
        DXSM::Vector3 Position;
        float Range;

        DXSM::Vector3 Direction;
        float Spot;

        DXSM::Vector3 Att;
        float pad;
    };

    struct SkyBoxData {
        DXSM::Vector3 Tint;
        float Power;
    };
}