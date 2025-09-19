#pragma once



#include "LightObject.h"

#include "Utils/ShapeGenerator.h"
//#include <DirectXCollision.h>

#include <d3d11.h>
#include <directxmath.h>

#include "GraphicsUtils/Camera.h"

class SpotLight :
    public LightObject
{
public:
    SpotLight(ID3D11Device* device, Vector3 position,
        float range, Vector3 direction, float spot, Vector3 att,
        Vector4 ambient, Vector4 diffuse, Vector4 specular);

    struct SpotLightPCB {
        XMFLOAT4 Diffuse;
        XMFLOAT4 Specular;
        XMFLOAT3 Position;
        float Range;

        XMFLOAT3 Direction;
        float Spot;

        XMFLOAT3 Att;
        float pad;
    } spotLightData;

    float width;
    float depth;

    Vector4 ambient;

    Bind::PixelConstantBuffer<SpotLightPCB>* spotLightPBuffer;

    D3D11_DEPTH_STENCIL_DESC GetDepthStencilDesc(LightObject::LightPosition lightPos) override;
    D3D11_RASTERIZER_DESC GetRasterizerDesc(LightObject::LightPosition lightPos) override;

    LightPosition GetLightPositionInFrustum(Camera* camera) override;
    bool IsFrustumInsideOfLight(Camera* camera) override;

    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;
    //BoundingOrientedBox();

    void UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
};

