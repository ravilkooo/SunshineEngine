#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"

class SkyBoxTechnique :
    public LightTechnique<SkyBoxData>
{
public:
    SkyBoxTechnique(ID3D11Device* device, eastl::string technique,
        eastl::shared_ptr<Camera> camera,
        eastl::shared_ptr<SkyBoxData> lightData);

    void ChooseDepthStencilState(LightPosition lightPos) override;
    void ChooseRasterizer(LightPosition lightPos) override;

    LightPosition GetLightPositionInFrustum() override;
    bool IsFrustumInsideOfLight() override;
};

