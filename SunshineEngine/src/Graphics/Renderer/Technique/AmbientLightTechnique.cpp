#include "Graphics/Renderer/Technique/AmbientLightTechnique.h"
#include <Graphics/GraphicsResources/Mesh.h>

#include <ResourceManager/ResourceManagerFacade.h>

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
        m_depthStencilState = eastl::make_unique<Bind::DepthStencilState>(device, dsDesc);

        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        m_rasterizer = eastl::make_unique<Bind::Rasterizer>(device, rasterDesc);

        AssetPath meshPath = AssetPath(L"ScreenAlignedQuad");
        auto& rm = ResourceManagerFacade::Instance();
        ResourceHandle meshHandle = rm.LoadByPath(meshPath);
        SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
        m_mesh = eastl::shared_ptr<SE_G::Mesh>(
            meshRes,
            [](SE_G::Mesh*) {}
        );
        m_mesh->m_meshPath = meshRes->m_meshPath;

        m_vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
            device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/AmbientLightVShader.hlsl").c_str());
        m_pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
            device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/AmbientLightPShader.hlsl").c_str());
    }

    void AmbientLightTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    {
        // to-do: update only when changed
        m_lightDataPixelCBuffer->Update(context.Get(), *m_lightData);
        BindAll(context);
        DrawTechnique(context);
    }

    void AmbientLightTechnique::ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos)
    {
        return;
    }

    void AmbientLightTechnique::ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos)
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