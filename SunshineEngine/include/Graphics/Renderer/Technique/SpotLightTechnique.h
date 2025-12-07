#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"

#include <Component/TransformComponent.h>

#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;
namespace DX = DirectX;

namespace SE_G {
    class SpotLightTechnique :
        public LightTechnique<SpotLightData>
    {
    public:
        SpotLightTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique,
            eastl::shared_ptr<Camera> camera,
            eastl::shared_ptr<SpotLightData> lightData);

        //void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        void Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        void ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos) override;
        void ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos) override;

        LightPosition GetLightPositionInFrustum() override;
        bool IsFrustumInsideOfLight() override;
    };
}
