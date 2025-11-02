#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <GraphicsResources/VertexShader.h>
#include "GameObject.h"

#include "LightObject.h"

#include "Utils/ShapeGenerator.h"

#include <directxmath.h>

#include "GraphicsUtils/Camera.h"

#include "Bindable/BindableCollection.h"
#include "Graphics/RenderTechnique.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

class SpotLight :
    public LightObject<SpotLightData>
{
public:
    /*
    SpotLight(ID3D11Device* device, DXSM::Vector3 position,
        float range, DXSM::Vector3 direction, float spot, DXSM::Vector3 att,
        DXSM::Vector4 ambient, DXSM::Vector4 diffuse, DXSM::Vector4 specular);

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

    D3D11_DEPTH_STENCIL_DESC ChooseDepthStencilState(LightObject::LightPosition lightPos) override;
    D3D11_RASTERIZER_DESC GetRasterizerDesc(LightObject::LightPosition lightPos) override;

    LightPosition GetLightPositionInFrustum(Camera* camera) override;
    bool IsFrustumInsideOfLight(Camera* camera) override;
    
    void UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
    */
};
