#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <GraphicsResources/VertexShader.h>
#include "GameObject.h"

namespace DXSM = DirectX::SimpleMath;

#include "LightObject.h"

class AmbientLight :
    public LightObject
{
    /*
public:
    AmbientLight(ID3D11Device* device, DXSM::Vector4 ambient);

    struct AmbientLightPCB {
        DirectX::XMFLOAT4 Ambient;
    } ambientLightData;

    DXSM::Vector4 ambient;

    Bind::PixelConstantBuffer<AmbientLightPCB>* ambientLightPBuffer;

    D3D11_DEPTH_STENCIL_DESC GetDepthStencilDesc(LightObject::LightPosition lightPos) override;
    D3D11_RASTERIZER_DESC GetRasterizerDesc(LightObject::LightPosition lightPos) override;

    LightPosition GetLightPositionInFrustum(Camera* camera) override;
    bool IsFrustumInsideOfLight(Camera* camera) override;

    void UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
    */
};


