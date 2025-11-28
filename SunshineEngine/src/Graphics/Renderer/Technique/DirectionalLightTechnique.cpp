#include <Graphics/Renderer/Technique/DirectionalLightTechnique.h>
#include <Graphics/Renderer/Pass/ShadowMapPass.h>

#include <Component/TransformComponent.h>

#include <Utils/StringUtils.h>

namespace SE_G {
    bool DirectionalLightTechnique::s_staticDataInitializated = false;
    eastl::unique_ptr<Bind::PixelShader> DirectionalLightTechnique::s_noShadowShader;
    eastl::unique_ptr<Bind::PixelShader> DirectionalLightTechnique::s_shadowShader;


    DirectionalLightTechnique::DirectionalLightTechnique(ID3D11Device* device,
        TransformComponent* assignedTransform,
        eastl::string technique,
        eastl::shared_ptr<Camera> camera,
        eastl::shared_ptr<DirectionalLightData> lightData)
        : LightTechnique(device, assignedTransform, technique, camera, lightData)
    {
        if (!DirectionalLightTechnique::s_staticDataInitializated)
        {
            DirectionalLightTechnique::InitStaticData(device);
        }

        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        m_depthStencilState = eastl::make_unique<Bind::DepthStencilState>(device, dsDesc);

        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        m_rasterizer = eastl::make_unique<Bind::Rasterizer>(device, rasterDesc);

        // Add mesh for Ambient
        m_mesh = SE_G::Mesh::CreateScreenAlignedQuad(device);
        m_vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
            device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/DirectionalLightVShader.hlsl").c_str());
    }

    void DirectionalLightTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    {
        // to-do: update only when changed
        m_lightData->Position = m_assignedTransform->m_position;
        m_lightDataBuffer->Update(context.Get(), *m_lightData);
        BindAll(context);

        if (m_castsShadow)
        {
            m_shadowMapPass->BindForLightingPass();
            s_shadowShader->Bind(context.Get());
        }
        else
        {
            s_noShadowShader->Bind(context.Get());
        }
        DrawTechnique(context);
    }

    void DirectionalLightTechnique::ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos)
    {
        return;
    }

    void DirectionalLightTechnique::ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos)
    {
        return;
    }

    LightPosition DirectionalLightTechnique::GetLightPositionInFrustum()
    {
        return LightPosition::FILL;
    }

    bool DirectionalLightTechnique::IsFrustumInsideOfLight()
    {
        return true;
    }

    void DirectionalLightTechnique::EnableShadow(ShadowMapPass* shadowMapPass)
    {
        m_shadowMapPass = shadowMapPass;
        m_castsShadow = true;
    }

    void DirectionalLightTechnique::DisableShadow()
    {
        m_castsShadow = false;
    }

    void DirectionalLightTechnique::InitStaticData(ID3D11Device* device)
    {
        s_noShadowShader = eastl::make_unique<SE_G::Bind::PixelShader>(
            device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/DirectionalLightPS.hlsl").c_str());
        s_shadowShader = eastl::make_unique<SE_G::Bind::PixelShader>(
            device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/DirectionalLightShadowPS.hlsl").c_str());

        s_staticDataInitializated = true;
    }
}