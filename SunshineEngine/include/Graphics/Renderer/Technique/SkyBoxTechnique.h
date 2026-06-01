#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"
#include <Utils/AssetPath.h>

namespace SE_G {
    class SkyBoxTechnique :
        public LightTechnique<SkyBoxData>
    {
    public:
        SkyBoxTechnique(DeferredRenderer* renderer,
            TransformComponent* assignedTransform, eastl::string technique,
            eastl::shared_ptr<SkyBoxData> lightData,
            AssetPath assetPath = AssetPath(eastl::wstring(L"Textures/DefaultSkybox.dds"), AssetPath::AssetSource::Engine));

        void BindAll(ID3D11DeviceContext* context) override;
        void Pass(ID3D11DeviceContext* context) override;

        void ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos) override;
        void ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos) override;

        LightPosition GetLightPositionInFrustum(Camera* camera) override;
        bool IsFrustumInsideOfLight(Camera* camera) override;
    };
}
