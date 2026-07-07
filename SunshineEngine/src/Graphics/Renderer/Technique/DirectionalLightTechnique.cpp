#include <Graphics/Renderer/Technique/DirectionalLightTechnique.h>
#include <Graphics/Renderer/Pass/ShadowMapPass.h>

#include <Component/TransformComponent.h>

#include <ResourceManager/ResourceManagerFacade.h>

#include <Utils/StringUtils.h>

namespace SE_G {
    bool DirectionalLightTechnique::s_staticDataInitializated = false;
    eastl::shared_ptr<Bind::PixelShader> DirectionalLightTechnique::s_noShadowShader;
    eastl::shared_ptr<Bind::PixelShader> DirectionalLightTechnique::s_shadowShader;


    DirectionalLightTechnique::DirectionalLightTechnique(DeferredRenderer* renderer,
        TransformComponent* assignedTransform,
        eastl::string technique,
        eastl::shared_ptr<DirectionalLightData> lightData)
        : LightTechnique(renderer, assignedTransform, technique, lightData)
    {
        if (!DirectionalLightTechnique::s_staticDataInitializated)
        {
            DirectionalLightTechnique::InitStaticData(renderer->GetDevice());
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

    void DirectionalLightTechnique::Pass(ID3D11DeviceContext* context)
    {
        // Save dirty flags, cause mesh editing marks dirty
        uint32_t dirtyFlags = m_assignedTransform->IsDirty();
        if (dirtyFlags | TransformComponent::DirtyFlags::LightPos)
        {
            m_lightData->Position = m_assignedTransform->GetAbsoluteWorldPosition();
            m_lightDataPixelCBuffer->Update(context, *m_lightData);

            dirtyFlags = dirtyFlags & ~TransformComponent::DirtyFlags::LightPos;
        }

        BindAll(context);

        if (m_castsShadow)
        {
            m_shadowMapPass->BindForLightingPass();
            s_shadowShader->Bind(context);
            DrawTechnique(context);
        }
        else
        {
            s_noShadowShader->Bind(context);
            DrawTechnique(context);
        }

        m_assignedTransform->SetDirty(dirtyFlags | TransformComponent::DirtyFlags::GPU);
    }

    void DirectionalLightTechnique::ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos)
    {
        LightStaticData::depthCompLess->Bind(context);
    }

    void DirectionalLightTechnique::ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos)
    {
        LightStaticData::rastCullNone->Bind(context);
    }

    LightPosition DirectionalLightTechnique::GetLightPositionInFrustum(Camera* camera)
    {
        return LightPosition::FILL;
    }

    bool DirectionalLightTechnique::IsFrustumInsideOfLight(Camera* camera)
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
        AssetPath shaderPath = AssetPath(L"Shaders/LightPass/DirectionalLightPS.hlsl", AssetPath::AssetSource::Engine);
        shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::PIXEL_SHADER;
        auto& rm = ResourceManagerFacade::Instance();
        ResourceHandle pshaderNoShadowHandle = rm.LoadByPath(shaderPath);
        SE_G::Bind::PixelShader* pshaderNoShadowRes = rm.Get<SE_G::Bind::PixelShader>(pshaderNoShadowHandle);
        s_noShadowShader = eastl::shared_ptr<SE_G::Bind::PixelShader>(
            pshaderNoShadowRes,
            [](SE_G::Bind::PixelShader*) {}
        );

        shaderPath = AssetPath(L"Shaders/LightPass/DirectionalLightShadowPS.hlsl", AssetPath::AssetSource::Engine);
        shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::PIXEL_SHADER;
        ResourceHandle pshaderHandle = rm.LoadByPath(shaderPath);
        SE_G::Bind::PixelShader* pshaderRes = rm.Get<SE_G::Bind::PixelShader>(pshaderHandle);
        s_shadowShader = eastl::shared_ptr<SE_G::Bind::PixelShader>(
            pshaderRes,
            [](SE_G::Bind::PixelShader*) {}
        );


        s_staticDataInitializated = true;
    }
}