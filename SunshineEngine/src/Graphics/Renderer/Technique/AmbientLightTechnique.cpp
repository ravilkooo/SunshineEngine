#include "Graphics/Renderer/Technique/AmbientLightTechnique.h"

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