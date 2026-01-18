#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"

#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;
namespace DX = DirectX;

class TransformComponent;

namespace SE_G {
    class PointLightTechnique :
        public LightTechnique<PointLightData>
    {
    public:
        PointLightTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique,
            eastl::shared_ptr<Camera> camera,
            eastl::shared_ptr<PointLightData> lightData);

        //void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        void Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        void ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos) override;
        void ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos) override;

        LightPosition GetLightPositionInFrustum() override;
        bool IsFrustumInsideOfLight() override;

    };
}
