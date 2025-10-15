#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"

class PointLightTechnique :
    public LightTechnique<PointLightData>
{
public:
    PointLightTechnique(ID3D11Device* device, eastl::string technique);

    void ChooseDepthStencilState(LightPosition lightPos) override;
    void ChooseRasterizer(LightPosition lightPos) override;

    LightPosition GetLightPositionInFrustum() override;
    bool IsFrustumInsideOfLight() override;

    eastl::shared_ptr<Bind::DepthStencilState> depthCompLess;
    eastl::shared_ptr<Bind::DepthStencilState> depthCompGreater;

    eastl::shared_ptr<Bind::Rasterizer> rastCullNone;
    eastl::shared_ptr<Bind::Rasterizer> rastCullBack;
    eastl::shared_ptr<Bind::Rasterizer> rastCullFront;
};

