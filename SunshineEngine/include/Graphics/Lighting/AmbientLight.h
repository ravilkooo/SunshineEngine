#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <GraphicsResources/VertexShader.h>
#include "GameObject.h"

#include "LightObject.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

class AmbientLight :
    public LightObject
{
public:
    AmbientLightData ambientLightData;

    AmbientLight(AmbientLightData initData = { DXSM::Vector4::One });

    // Unnecessary?
    //void UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
/*
    DXSM::Vector4 ambient;

    Bind::PixelConstantBuffer<AmbientLightPCB>* ambientLightPBuffer;

    D3D11_DEPTH_STENCIL_DESC ChooseDepthStencilState(LightObject::LightPosition lightPos) override;
    D3D11_RASTERIZER_DESC GetRasterizerDesc(LightObject::LightPosition lightPos) override;
    */
};


