#pragma once


#include <d3d11.h>
#include <SimpleMath.h>
#include <GraphicsResources/VertexShader.h>
#include "GameObject.h"

#include "LightObject.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

class DirectionalLight :
    public LightObject
{
public:

    DirectionalLightData directionalLightData;

    DirectionalLight(ID3D11Device* device,
        DirectionalLightData directionalLightData =
        {
            DXSM::Vector4::One,
            DXSM::Vector4::One,
            DXSM::Vector3::Zero, 0,
            DXSM::Vector3::One, 0
        });

    // LightPosition GetLightPositionInFrustum(Camera* camera) override;
    // bool IsFrustumInsideOfLight(Camera* camera) override;

    // Unnecessary?
    //void UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

    /*
    Vector4 ambient;

    Bind::PixelConstantBuffer<DirectionalLightPCB>* directionalLightPBuffer;

    D3D11_DEPTH_STENCIL_DESC ChooseDepthStencilState(LightObject::LightPosition lightPos) override;
    D3D11_RASTERIZER_DESC GetRasterizerDesc(LightObject::LightPosition lightPos) override;

    */
};

