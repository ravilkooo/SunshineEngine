#pragma once
#include "LightTechnique.h"
#include "Graphics/Lighting/LightData.h"

namespace SE_G {
    class ShadowMapPass;

    class DirectionalLightTechnique :
        public LightTechnique<DirectionalLightData>
    {
    public:
        DirectionalLightTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique,
            eastl::shared_ptr<Camera> camera,
            eastl::shared_ptr<DirectionalLightData> lightData);

        //void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
        void Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        void ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos) override;
        void ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos) override;

        LightPosition GetLightPositionInFrustum() override;
        bool IsFrustumInsideOfLight() override;

        void AssignShadowMapPass(ShadowMapPass* shadowMapPass);
        void EnableShadow();
        void DisableShadow();

        bool m_castsShadow = false;
        ShadowMapPass* m_shadowMapPass;
    private:
        static void InitStaticData(ID3D11Device* device);

        static bool s_staticDataInitializated;

        static eastl::unique_ptr<Bind::PixelShader> s_noShadowShader;
        static eastl::unique_ptr<Bind::PixelShader> s_shadowShader;
    };
}
