#include <Graphics/Renderer/Technique/DirectionalLightTechnique.h>
#include <Graphics/Renderer/Pass/ShadowMapPass.h>

#include <Component/TransformComponent.h>

#include <ResourceManager/ResourceManagerFacade.h>

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

        AssetPath meshPath = AssetPath(L"ScreenAlignedQuad");
        auto& rm = ResourceManagerFacade::Instance();
        ResourceHandle meshHandle = rm.LoadByPath(meshPath);
        SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
        m_mesh = eastl::shared_ptr<SE_G::Mesh>(
            meshRes,
            [](SE_G::Mesh*) {}
        );
        m_mesh->m_meshPath = meshRes->m_meshPath;

        AssetPath shaderPath = AssetPath(L"Shaders/LightPass/DirectionalLightVShader.hlsl", AssetPath::AssetSource::Engine);
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
        //     device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/DirectionalLightVShader.hlsl").c_str());
    }

    void DirectionalLightTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    {
        // to-do: update only when changed
        auto wMat = m_assignedTransform->GetWorldMatrix();
        m_lightData->Position = DXSM::Vector3(wMat._41, wMat._42, wMat._43);
        m_lightDataPixelCBuffer->Update(context.Get(), *m_lightData);
        BindAll(context);

        if (m_castsShadow)
        {
            m_shadowMapPass->BindForLightingPass();
            s_shadowShader->Bind(context.Get());
            DrawTechnique(context);
        }
        else
        {
            s_noShadowShader->Bind(context.Get());
            DrawTechnique(context);
        }
    }

    void DirectionalLightTechnique::ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos)
    {
        LightStaticData::depthCompLess->Bind(context);
    }

    void DirectionalLightTechnique::ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos)
    {
        LightStaticData::rastCullNone->Bind(context);
    }

    LightPosition DirectionalLightTechnique::GetLightPositionInFrustum()
    {
        return LightPosition::FILL;
    }

    bool DirectionalLightTechnique::IsFrustumInsideOfLight()
    {
        return true;
    }

    void DirectionalLightTechnique::AssignShadowMapPass(ShadowMapPass* shadowMapPass)
    {
        m_shadowMapPass = shadowMapPass;
    }

    void DirectionalLightTechnique::EnableShadow()
    {
        if (m_shadowMapPass)
            m_castsShadow = true;
        else
            printf("No asssigned shadowMapPass"); // LOG_GAME(...)
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