#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"

namespace SE_G {
    class SkyBoxTechnique :
        public LightTechnique<SkyBoxData>
    {
    public:
        SkyBoxTechnique(ID3D11Device* device,
            TransformComponent* assignedTransform, eastl::string technique,
            eastl::shared_ptr<Camera> camera,
            eastl::shared_ptr<SkyBoxData> lightData,
            eastl::wstring texturePath = eastl::wstring(L"Default"));

        //void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        void ChooseDepthStencilState(LightPosition lightPos) override;
        void ChooseRasterizer(LightPosition lightPos) override;

        LightPosition GetLightPositionInFrustum() override;
        bool IsFrustumInsideOfLight() override;
    };
}
