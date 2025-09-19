#pragma once



#include "LightObject.h"

class DirectionalLight :
    public LightObject
{
public:
    DirectionalLight(ID3D11Device* device, Vector3 position, Vector3 direction, Vector4 ambient, Vector4 diffuse, Vector4 specular);

    struct DirectionalLightPCB {
        XMFLOAT4 Diffuse;
        XMFLOAT4 Specular;
        XMFLOAT3 Position;
        float pad1;

        XMFLOAT3 Direction;
        float pad2;
    } directionalLightData;

    Vector4 ambient;

    Bind::PixelConstantBuffer<DirectionalLightPCB>* directionalLightPBuffer;

    D3D11_DEPTH_STENCIL_DESC GetDepthStencilDesc(LightObject::LightPosition lightPos) override;
    D3D11_RASTERIZER_DESC GetRasterizerDesc(LightObject::LightPosition lightPos) override;

    LightPosition GetLightPositionInFrustum(Camera* camera) override;
    bool IsFrustumInsideOfLight(Camera* camera) override;

    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;

    void UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
};

