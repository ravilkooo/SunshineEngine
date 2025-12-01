#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <directxmath.h>

#include "Graphics/Utils/ShapeGenerator.h"
#include "Graphics/Utils/Camera.h"
#include "Graphics/Bindable/BindableCollection.h"
#include "Graphics/Renderer/Technique/RenderTechnique.h"
#include <Graphics/Lighting/LightData.h>

#include <GameObject/GameObject.h>
#include <GameObject/Lighting/LightObject.h>

#include <Serialization/LightDataSerialization.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace DXSM = DirectX::SimpleMath;


class SpotLight :
    public LightObject<SE_G::SpotLightData>
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