#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"

namespace SE_G {
    class AmbientLightTechnique :
        public LightTechnique<AmbientLightData>
    {
    public:
        AmbientLightTechnique(DeferredRenderer* renderer, TransformComponent* assignedTransform,
            eastl::string technique,
            eastl::shared_ptr<AmbientLightData> lightData);

        void Pass(ID3D11DeviceContext* context) override;

        void ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos) override;
        void ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos) override;

        LightPosition GetLightPositionInFrustum(Camera* camera) override;
        bool IsFrustumInsideOfLight(Camera* camera) override;
    };
}
