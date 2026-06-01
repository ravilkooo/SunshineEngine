#include "Graphics/Renderer/Technique/SkyBoxTechnique.h"
#include <Graphics/GraphicsResources/Mesh.h>
#include <Utils/StringUtils.h>
#include <Component/TransformComponent.h>

#include <ResourceManager/ResourceManagerFacade.h>

namespace SE_G {
    SkyBoxTechnique::SkyBoxTechnique(DeferredRenderer* renderer, TransformComponent* assignedTransform,
        eastl::string technique,
        eastl::shared_ptr<SkyBoxData> lightData,
        AssetPath assetPath)
        : LightTechnique(renderer, assignedTransform, technique, lightData) {
        auto device = m_renderer->GetDevice();

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

        auto& rm = ResourceManagerFacade::Instance();
        ResourceHandle texHandle = rm.LoadByPath(assetPath);
        if (texHandle.guid == 0) {
            // Error
            auto ap = AssetPath(
                SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
                AssetPath::AssetSource::Engine);
            texHandle = ResourceManagerFacade::Instance().LoadByPath(ap);
        }

        SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

        m_texture = eastl::shared_ptr<SE_G::Bind::Texture>(
            texRes,
            [](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
        m_texture->SetSlot(4u);

        m_textureSampler = eastl::make_shared<SE_G::Bind::Sampler>(
            device,
            CD3D11_SAMPLER_DESC(CD3D11_DEFAULT{}),
            1u,
            SE_G::Bind::PipelineStage::PIXEL_SHADER
        );

        // Add mesh for Ambient
        AssetPath meshPath = AssetPath(L"Box");
        ResourceHandle meshHandle = rm.LoadByPath(meshPath);
        SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
        m_mesh = eastl::shared_ptr<SE_G::Mesh>(
            meshRes,
            [](SE_G::Mesh*) {}
        );
        m_mesh->m_meshPath = meshRes->m_meshPath;

        AssetPath shaderPath = AssetPath(L"Shaders/LightPass/SkyBoxVShader.hlsl", AssetPath::AssetSource::Engine);
        shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::VERTEX_SHADER;
        SE_G::Bind::VertexShader::FillStandartInputLayout(shaderPath.m_params.asShader.numInputElements,
            shaderPath.m_params.asShader.IALayoutInputElements);
        ResourceHandle vshaderHandle = rm.LoadByPath(shaderPath);
        SE_G::Bind::VertexShader* vshaderRes = rm.Get<SE_G::Bind::VertexShader>(vshaderHandle);
        m_vertexShader = eastl::shared_ptr<SE_G::Bind::VertexShader>(
            vshaderRes,
            [](SE_G::Bind::VertexShader*) {}
        );
        delete[] shaderPath.m_params.asShader.IALayoutInputElements;
        // m_vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
        //     device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/SkyBoxVShader.hlsl").c_str());

        // m_pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
        //     device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/SkyBoxPShader.hlsl").c_str());

        shaderPath = AssetPath(L"Shaders/LightPass/SkyBoxPShader.hlsl", AssetPath::AssetSource::Engine);
        shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::PIXEL_SHADER;
        ResourceHandle pshaderHandle = rm.LoadByPath(shaderPath);
        SE_G::Bind::PixelShader* pshaderRes = rm.Get<SE_G::Bind::PixelShader>(pshaderHandle);
        m_pixelShader = eastl::shared_ptr<SE_G::Bind::PixelShader>(
            pshaderRes,
            [](SE_G::Bind::PixelShader*) {}
        );
    }

    void SkyBoxTechnique::BindAll(ID3D11DeviceContext* context)
    {
        for (size_t i = 0; i < m_bindables.size(); i++)
        {
            m_bindables[i]->Bind(context);
        }

        if (m_vertexShader) {
            m_vertexShader->Bind(context);
        }

        if (m_pixelShader) {
            m_pixelShader->Bind(context);
        }

        if (m_texture) {
            m_texture->Bind(context, 4u);
        }

        if (m_textureSampler) {
            m_textureSampler->Bind(context);
        }

        if (m_lightDataPixelCBuffer) {
            m_lightDataPixelCBuffer->Bind(context);
        }

        if (m_blendState)
            m_blendState->Bind(context);

        if (m_mesh)
            m_mesh->Bind(context);

        LightPosition lightPos = GetLightPositionInFrustum(m_renderer->GetMainCamera().get());
        // Choose rasterizer
        ChooseRasterizer(context, lightPos);
        // Choose depthState
        ChooseDepthStencilState(context, lightPos);

        // Bind rasterizer
        if (m_rasterizer)
            m_rasterizer->Bind(context);

        // Bind depthState
        if (m_depthStencilState)
            m_depthStencilState->Bind(context);
    }

    void SkyBoxTechnique::Pass(ID3D11DeviceContext* context)
    {
        // to-do: update only when changed
        m_lightDataPixelCBuffer->Update(context, *m_lightData);
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

    LightPosition SkyBoxTechnique::GetLightPositionInFrustum(Camera* camera)
    {
        return LightPosition::FILL;
    }

    bool SkyBoxTechnique::IsFrustumInsideOfLight(Camera* camera)
    {
        return true;
    }
}