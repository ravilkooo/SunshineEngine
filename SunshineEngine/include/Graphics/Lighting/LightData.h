#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;

struct AmbientLightData {
    DXSM::Vector4 Ambient;
};

struct DirectionalLightData {
    DXSM::Vector4 Diffuse;
    DXSM::Vector4 Specular;
    DXSM::Vector3 Position;
    float pad1;

    DXSM::Vector3 Direction;
    float pad2;
};

struct PointLightData {
    DXSM::Vector4 Diffuse;
    DXSM::Vector4 Specular;
    DXSM::Vector3 Position;
    float Range;

    DXSM::Vector3 Att;
    float pad;
};

struct SpotLightData {
    DXSM::Vector4 Diffuse;
    DXSM::Vector4 Specular;
    DXSM::Vector3 Position;
    float Range;

    DXSM::Vector3 Direction;
    float Spot;

    DXSM::Vector3 Att;
    float pad;
};

struct SkyBoxData {
    DXSM::Vector3 Tint;
    float brightness;
};
