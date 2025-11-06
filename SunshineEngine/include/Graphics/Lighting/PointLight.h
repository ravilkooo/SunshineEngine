#pragma once



#include <d3d11.h>
#include <SimpleMath.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include <GameObject.h>

#include <EASTL/algorithm.h>

#include "LightObject.h"

#include <Graphics/Utils/ShapeGenerator.h>
//#include <DirectXCollision.h>

#include <d3d11.h>
#include <directxmath.h>

#include <EASTL/shared_ptr.h>

#include "Graphics/Utils/Camera.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

namespace SE_G {
    class PointLight :
        public LightObject<PointLightData>
    {
    public:

        PointLight(
            PointLightData pointLightData =
            {
                DXSM::Vector3::One, 1,
                DXSM::Vector3::One, 1,
                DXSM::Vector3::Zero, 20,
                DXSM::Vector3::One, 0
            });

        // Unnecessary?
        //void UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        /*
        Vector4 ambient;

        Bind::PixelConstantBuffer<PointLightPCB>* pointLightPBuffer;

        D3D11_DEPTH_STENCIL_DESC ChooseDepthStencilState(LightObject::LightPosition lightPos) override;
        D3D11_RASTERIZER_DESC GetRasterizerDesc(LightObject::LightPosition lightPos) override;
        */
    };
}