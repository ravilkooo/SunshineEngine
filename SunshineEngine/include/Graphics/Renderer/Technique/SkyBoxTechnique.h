#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"
#include <Utils/AssetPath.h>

namespace SE_G {
    class SkyBoxTechnique :
        public LightTechnique<SkyBoxData>
    {
    public:
        SkyBoxTechnique(ID3D11Device* device,
            TransformComponent* assignedTransform, eastl::string technique,
            eastl::shared_ptr<Camera> camera,
            eastl::shared_ptr<SkyBoxData> lightData,
            AssetPath assetPath = AssetPath(L"DefaultSkybox.dds", AssetPath::AssetSource::Engine));

        //void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
        void Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        void ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos) override;
        void ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos) override;

        LightPosition GetLightPositionInFrustum() override;
        bool IsFrustumInsideOfLight() override;
    };
}
