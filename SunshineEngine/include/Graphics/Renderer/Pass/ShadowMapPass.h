#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <wrl.h>

#include <Graphics/Renderer/Pass/RenderPass.h>
#include <Graphics/Renderer/Pass/GPass.h>

#include <Graphics/Renderer/ShadowMap.h>

namespace SE_G {
    class ShadowMapPass :
        public RenderPass
    {
    public:

        struct ShadowTransformData
        {
            DX::XMMATRIX lightView;
            DX::XMMATRIX lightProjection;
            DX::XMMATRIX shadowTransformFull;
        };
        struct CascadesData {
            ShadowTransformData cascades[4];
            DX::XMFLOAT4 distances;
        };

        ShadowMapPass(ID3D11Device* device, ID3D11DeviceContext* context,
            GPass* gPass,
            eastl::shared_ptr<SE_G::DirectionalLightData> lightData,
            UINT smSizeX = 1024,
            UINT smSizeY = 1024);
        ~ShadowMapPass();

        void StartFrame() override;
        void Pass() override;
        void EndFrame() override;

        SE_G::Camera* GetFrustumCamera();
        void GetFrustumBoundsZ(int i, float* nearZ, float* farZ);

        ShadowTransformData GenerateBoundingFrustum(UINT cascadeNum);
        void MapCurrentCascadeData();

        void BindForLightingPass();

        GPass* m_gPass;
        ShadowMap m_shadowMap;
        
        UINT currCascade = 0;

        float cascadeBounds[5] = { 0.1f, 10.0f, 30.0f, 80.0f, 200.0f };
        // To-do: get formula for this value
        float frustumBias = 1.0f;

        CascadesData m_cascadesData;

        D3D11_VIEWPORT m_smViewport;

        eastl::unique_ptr<SE_G::Bind::VertexShader> vertexShader;

        eastl::unique_ptr<SE_G::Bind::VertexConstantBuffer<ShadowTransformData>> m_shadowTransformsConstantBuffer;

        eastl::shared_ptr<SE_G::DirectionalLightData> m_lightData;
        eastl::shared_ptr<SE_G::Bind::PixelConstantBuffer<SE_G::DirectionalLightData>> m_lightDataBuffer;
        eastl::unique_ptr<SE_G::Camera> m_lightViewCamera;
        SE_G::Camera* m_playerCamera;

        // For lighting
        eastl::unique_ptr<Bind::Texture> m_shadowMapTexture;
        eastl::unique_ptr<Bind::Sampler> m_shadowSampler_1;
        eastl::unique_ptr<Bind::Sampler> m_shadowSampler_2;

        eastl::unique_ptr<SE_G::Bind::PixelConstantBuffer<CascadesData>> m_cascadesConstantBuffer;
    };
}
