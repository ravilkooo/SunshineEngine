#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"

namespace SE_G {
    class DirectionalLightTechnique :
        public LightTechnique<DirectionalLightData>
    {
    public:
        DirectionalLightTechnique(ID3D11Device* device, eastl::string technique,
            eastl::shared_ptr<Camera> camera,
            eastl::shared_ptr<DirectionalLightData> lightData);

        void ChooseDepthStencilState(LightPosition lightPos) override;
        void ChooseRasterizer(LightPosition lightPos) override;

        LightPosition GetLightPositionInFrustum() override;
        bool IsFrustumInsideOfLight() override;
    };
}
