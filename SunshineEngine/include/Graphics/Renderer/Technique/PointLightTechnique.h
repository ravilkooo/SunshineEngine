#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"

#include <Component/TransformComponent.h>

#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;
namespace DX = DirectX;

namespace SE_G {
    class PointLightTechnique :
        public LightTechnique<PointLightData>
    {
    public:
        PointLightTechnique(ID3D11Device* device, eastl::string technique,
            eastl::shared_ptr<Camera> camera,
            eastl::shared_ptr<PointLightData> lightData);

        //void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        void Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        void ChooseDepthStencilState(LightPosition lightPos) override;
        void ChooseRasterizer(LightPosition lightPos) override;

        LightPosition GetLightPositionInFrustum() override;
        bool IsFrustumInsideOfLight() override;

        eastl::shared_ptr<Bind::DepthStencilState> depthCompLess;
        eastl::shared_ptr<Bind::DepthStencilState> depthCompGreater;

        eastl::shared_ptr<Bind::Rasterizer> rastCullNone;
        eastl::shared_ptr<Bind::Rasterizer> rastCullBack;
        eastl::shared_ptr<Bind::Rasterizer> rastCullFront;

        eastl::shared_ptr<TransformComponent> m_assignedTransform;
    };
}
