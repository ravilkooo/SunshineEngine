#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"

class AmbientLightTechnique :
    public LightTechnique<AmbientLightData>
{
public:
    AmbientLightTechnique(ID3D11Device* device, eastl::string technique);

    void ChooseDepthStencilState(LightPosition lightPos) override;
    void ChooseRasterizer(LightPosition lightPos) override;

    LightPosition GetLightPositionInFrustum() override;
    bool IsFrustumInsideOfLight() override;
};

