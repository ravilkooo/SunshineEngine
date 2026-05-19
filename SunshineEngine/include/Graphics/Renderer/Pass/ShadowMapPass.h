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

        float cascadeBounds[5] = { 0.01f, 8.0f, 20.0f, 80.0f, 200.0f };
        // To-do: get formula for this value
        float frustumBias = 7.0f;

        CascadesData m_cascadesData;

        D3D11_VIEWPORT m_smViewport;

        eastl::unique_ptr<Bind::Rasterizer> m_shadowMapRasterizer;
        eastl::shared_ptr<SE_G::Bind::VertexShader> vertexShader;

        eastl::unique_ptr<SE_G::Bind::VertexConstantBuffer<ShadowTransformData>> m_shadowTransformsConstantBuffer;

        eastl::shared_ptr<SE_G::DirectionalLightData> m_lightData;
        eastl::shared_ptr<SE_G::Bind::PixelConstantBuffer<SE_G::DirectionalLightData>> m_lightDataBuffer;
        eastl::unique_ptr<SE_G::Camera> m_lightViewCamera;
        SE_G::Camera* m_playerCamera;

        struct MapSizePCB {
            DXSM::Vector2 MapSize;
        };
        eastl::unique_ptr<Bind::PixelConstantBuffer<MapSizePCB>> m_mapSizePCB;

        // For lighting
        eastl::unique_ptr<Bind::Texture> m_shadowMapTexture;
        eastl::unique_ptr<Bind::Sampler> m_shadowSampler_1;
        eastl::unique_ptr<Bind::Sampler> m_shadowSampler_2;

        eastl::unique_ptr<SE_G::Bind::PixelConstantBuffer<CascadesData>> m_cascadesConstantBuffer;

        struct FrustumPoints
        {
            DXSM::Vector4 corners[8];
            // corner_mm0, float _pad_mm0;
            // corner_mm1, float _pad_mm1;
            // corner_mp0, float _pad_mp0;
            // corner_mp1, float _pad_mp1;
            // corner_pm0, float _pad_pm0;
            // corner_pm1, float _pad_pm1;
            // corner_pp0, float _pad_pp0;
            // corner_pp1, float _pad_pp1;
            UINT idx = 0u;
        };
        FrustumPoints FillFrustumPoints(Camera* camera);

        // temporary mesh for viewing frustums
        /*
        void DrawFrustums();
    private:
        eastl::shared_ptr<Mesh> m_frustumCube;

        eastl::unique_ptr<Bind::VertexShader> m_frusumVS;
        eastl::unique_ptr<Bind::PixelShader> m_frusumPS;
        eastl::unique_ptr<Bind::Rasterizer> m_rasterizer;
        eastl::unique_ptr<Bind::BlendState> m_blendFrust;

        FrustumPoints fps[4];

        eastl::unique_ptr<Bind::VertexConstantBuffer<FrustumPoints>> m_fpBuffer;

        eastl::unique_ptr<Bind::DepthStencilState> m_depthStencilFrust;

        void InitFrustumStuff(ID3D11Device* device, ID3D11DeviceContext* context);
        */
    };
}
