#pragma once

#include <EASTL/algorithm.h>

#include "LightObject.h"

#include "Utils/ShapeGenerator.h"
//#include <DirectXCollision.h>

#include <d3d11.h>
#include <directxmath.h>

#include "GraphicsUtils/Camera.h"

class PointLight :
    public LightObject
{
public:
    PointLight();
    PointLight(ID3D11Device* device, Vector3 position,
        float range, Vector3 att, Vector4 ambient, Vector4 diffuse, Vector4 specular);

    struct PointLightPCB {
        XMFLOAT4 Diffuse;
        XMFLOAT4 Specular;
        XMFLOAT3 Position;
        float Range;

        XMFLOAT3 Att;
        float pad;
    } pointLightData;

    Vector4 ambient;

    Bind::PixelConstantBuffer<PointLightPCB>* pointLightPBuffer;

    D3D11_DEPTH_STENCIL_DESC GetDepthStencilDesc(LightObject::LightPosition lightPos) override;
    D3D11_RASTERIZER_DESC GetRasterizerDesc(LightObject::LightPosition lightPos) override;

    LightPosition GetLightPositionInFrustum(Camera* camera) override;
    bool IsFrustumInsideOfLight(Camera* camera) override;

    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;

    void UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
};
