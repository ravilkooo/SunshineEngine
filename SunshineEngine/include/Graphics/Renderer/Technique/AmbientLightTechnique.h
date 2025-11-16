#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"

namespace SE_G {
    class AmbientLightTechnique :
        public LightTechnique<AmbientLightData>
    {
    public:
        AmbientLightTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique,
            eastl::shared_ptr<Camera> camera,
            eastl::shared_ptr<AmbientLightData> lightData);

        void ChooseDepthStencilState(LightPosition lightPos) override;
        void ChooseRasterizer(LightPosition lightPos) override;

        LightPosition GetLightPositionInFrustum() override;
        bool IsFrustumInsideOfLight() override;
    };
}
