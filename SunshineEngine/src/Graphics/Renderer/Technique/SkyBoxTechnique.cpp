#include "Graphics/Renderer/Technique/SkyBoxTechnique.h"
#include <Utils/StringUtils.h>
#include <Component/TransformComponent.h>

namespace SE_G {
    SkyBoxTechnique::SkyBoxTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
        eastl::string technique,
        eastl::shared_ptr<Camera> camera,
        eastl::shared_ptr<SkyBoxData> lightData,
        eastl::wstring texturePath)
        : LightTechnique(device, assignedTransform, technique, camera, lightData) {

        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilState = eastl::make_shared<Bind::DepthStencilState>(device, dsDesc);

        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.CullMode = D3D11_CULL_FRONT;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterizer = eastl::make_shared<Bind::Rasterizer>(device, rasterDesc);

        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        blendState = eastl::make_shared<Bind::BlendState>(device, blendDesc);

        if (texturePath.empty() || texturePath == L"Default") {
            m_texture = eastl::make_shared<SE_G::Bind::Texture>(
                device,
                MakeEngineAssetPath_Wchar(L"DefaultSkybox.dds"),
                4u,
                SE_G::Bind::PipelineStage::PIXEL_SHADER
            );
        }
        else
        {
            m_texture = eastl::make_shared<SE_G::Bind::Texture>(
                device,
                texturePath,
                4u,
                SE_G::Bind::PipelineStage::PIXEL_SHADER
            );
        }

        m_textureSampler = eastl::make_shared<SE_G::Bind::Sampler>(
            device,
            CD3D11_SAMPLER_DESC(CD3D11_DEFAULT{}),
            1u,
            SE_G::Bind::PipelineStage::PIXEL_SHADER
        );

        float toHalfDiag = 1.15; // 2 / sqrt(3) = 1,15470053838
        // Add mesh for Ambient
        m_mesh = SE_G::Mesh::CreateUnwrappedBoxMesh(device,
            DXSM::Vector3::One * camera->GetFarZ() * toHalfDiag);

        m_vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
            device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/SkyBoxVShader.hlsl"));

        m_pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
            device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/SkyBoxPShader.hlsl"));

    }

    void SkyBoxTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    {
        // to-do: update only when changed
        m_lightDataBuffer->Update(context.Get(), *m_lightData);
        BindAll(context);
        DrawTechnique(context);
    }

    void SkyBoxTechnique::ChooseDepthStencilState(LightPosition lightPos)
    {
        return;
    }

    void SkyBoxTechnique::ChooseRasterizer(LightPosition lightPos)
    {
        return;
    }

    LightPosition SkyBoxTechnique::GetLightPositionInFrustum()
    {
        return LightPosition::FILL;
    }

    bool SkyBoxTechnique::IsFrustumInsideOfLight()
    {
        return true;
    }
}