#include "Graphics/Renderer/Technique/AmbientLightTechnique.h"
#include <Utils/StringUtils.h>

namespace SE_G {
    AmbientLightTechnique::AmbientLightTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique,
        eastl::shared_ptr<Camera> camera,
        eastl::shared_ptr<AmbientLightData> lightData)
        : LightTechnique(device, assignedTransform, technique, camera, lightData) {

        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilState = eastl::make_shared<Bind::DepthStencilState>(device, dsDesc);

        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterizer = eastl::make_shared<Bind::Rasterizer>(device, rasterDesc);

        // Add mesh for Ambient
        m_mesh = SE_G::Mesh::CreateScreenAlignedQuad(device);
        m_vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
            device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/AmbientLightVShader.hlsl"));
        m_pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
            device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/AmbientLightPShader.hlsl"));
    }

    void AmbientLightTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    {
        // to-do: update only when changed
        m_lightDataBuffer->Update(context.Get(), *m_lightData);
        BindAll(context);
        DrawTechnique(context);
    }

    void AmbientLightTechnique::ChooseDepthStencilState(LightPosition lightPos)
    {
        return;
    }

    void AmbientLightTechnique::ChooseRasterizer(LightPosition lightPos)
    {
        return;
    }

    LightPosition AmbientLightTechnique::GetLightPositionInFrustum()
    {
        return LightPosition::FILL;
    }

    bool AmbientLightTechnique::IsFrustumInsideOfLight()
    {
        return true;
    }
}