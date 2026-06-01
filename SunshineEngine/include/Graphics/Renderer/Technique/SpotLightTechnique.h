#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"

#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;
namespace DX = DirectX;

class TransformComponent;

namespace SE_G {
    class SpotLightTechnique :
        public LightTechnique<SpotLightData>
    {
    public:
        SpotLightTechnique(DeferredRenderer* renderer, TransformComponent* assignedTransform,
            eastl::string technique,
            eastl::shared_ptr<SpotLightData> lightData);

        //void BindAll(ID3D11DeviceContext* context) override;

        void Pass(ID3D11DeviceContext* context) override;

        void ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos) override;
        void ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos) override;

        LightPosition GetLightPositionInFrustum(Camera* camera) override;
        bool IsFrustumInsideOfLight(Camera* camera) override;
    };
}
