#pragma once


#include <d3d11.h>
#include <SimpleMath.h>
#include <EASTL/shared_ptr.h>
#include <GraphicsResources/VertexShader.h>
#include "GameObject.h"

#include "LightObject.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

class DirectionalLight :
    public LightObject<DirectionalLightData>
{
public:

    DirectionalLight(
        DirectionalLightData directionalLightData =
        {
            DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
            DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
            DXSM::Vector3::Zero, 0,
            DXSM::Vector3(0, -1, 0), 0
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

