#include "Graphics/Renderer/Technique/SkyBoxTechnique.h"
#include <Graphics/GraphicsResources/Mesh.h>
#include <Utils/StringUtils.h>
#include <Component/TransformComponent.h>

#include <ResourceManager/ResourceManagerFacade.h>

namespace SE_G {
    SkyBoxTechnique::SkyBoxTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
        eastl::string technique,
        eastl::shared_ptr<Camera> camera,
        eastl::shared_ptr<SkyBoxData> lightData,
        AssetPath assetPath)
        : LightTechnique(device, assignedTransform, technique, camera, lightData) {

        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        m_blendState = eastl::make_unique<Bind::BlendState>(device, blendDesc);

        /*
        m_texture = eastl::make_shared<SE_G::Bind::Texture>(
            device,
            assetPath,
            4u,
            SE_G::Bind::PipelineStage::PIXEL_SHADER
        );
        */

        auto& rm = ResourceManagerFacade::Instance();
        ResourceHandle texHandle = rm.LoadByPath(assetPath);
        SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

        if (texRes)
        {
            m_texture = eastl::shared_ptr<SE_G::Bind::Texture>(
                texRes,
                [](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
            m_texture->SetSlot(4u);
        }
        else
        {
            m_texture = eastl::make_shared<SE_G::Bind::Texture>(
                device,
                assetPath,
                4u,
                SE_G::Bind::PipelineStage::PIXEL_SHADER);
        }

        m_textureSampler = eastl::make_shared<SE_G::Bind::Sampler>(
            device,
            CD3D11_SAMPLER_DESC(CD3D11_DEFAULT{}),
            1u,
            SE_G::Bind::PipelineStage::PIXEL_SHADER
        );

        // Add mesh for Ambient
        m_mesh = SE_G::Mesh::CreateUnwrappedBoxMesh(device);

        m_vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
            device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/SkyBoxVShader.hlsl").c_str());

        m_pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
            device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/SkyBoxPShader.hlsl").c_str());

    }

    void SkyBoxTechnique::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    {
        for (size_t i = 0; i < m_bindables.size(); i++)
        {
            m_bindables[i]->Bind(context.Get());
        }

        if (m_vertexShader) {
            m_vertexShader->Bind(context.Get());
        }

        if (m_pixelShader) {
            m_pixelShader->Bind(context.Get());
        }

        if (m_texture) {
            m_texture->Bind(context.Get(), 4u);
        }

        if (m_textureSampler) {
            m_textureSampler->Bind(context.Get());
        }

        if (m_lightDataPixelCBuffer) {
            m_lightDataPixelCBuffer->Bind(context.Get());
        }

        if (m_blendState)
            m_blendState->Bind(context.Get());

        if (m_mesh)
            m_mesh->Bind(context.Get());

        LightPosition lightPos = GetLightPositionInFrustum();
        // Choose rasterizer
        ChooseRasterizer(context.Get(), lightPos);
        // Choose depthState
        ChooseDepthStencilState(context.Get(), lightPos);

        // Bind rasterizer
        if (m_rasterizer)
            m_rasterizer->Bind(context.Get());

        // Bind depthState
        if (m_depthStencilState)
            m_depthStencilState->Bind(context.Get());
    }

    void SkyBoxTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    {
        // to-do: update only when changed
        m_lightDataPixelCBuffer->Update(context.Get(), *m_lightData);
        BindAll(context);
        DrawTechnique(context);
    }

    void SkyBoxTechnique::ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos)
    {
        LightStaticData::depthCompLess->Bind(context);
    }

    void SkyBoxTechnique::ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos)
    {
        LightStaticData::rastCullFront->Bind(context);
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