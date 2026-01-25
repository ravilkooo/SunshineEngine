#pragma once
#include "RenderPass.h"
#include <Graphics/Renderer/GBuffer.h>
#include <Graphics/Renderer/Technique/ColliderTechnique.h>

#include <Graphics/Utils/Camera.h>

#include <Graphics/Bindable/DepthStencilState.h>

namespace SE_G {


    class ColliderPass :
        public RenderPass
    {
    public:
        ColliderPass(ID3D11Device* device, ID3D11DeviceContext* context,
            eastl::shared_ptr<GBuffer> pGBuffer,
            eastl::shared_ptr<Camera> camera);
        ~ColliderPass();

        // Inherited via RenderPass
        void StartFrame() override;
        void Pass() override;
        void EndFrame() override;

        void OnResize(UINT resizeWidth, UINT resizeHeight) override;
        //eastl::shared_ptr<GBuffer> pGBuffer);

        UINT m_screenWidth = 800u;
        UINT m_screenHeight = 800u;

        eastl::shared_ptr<GBuffer> m_GBuffer;

        ID3D11RenderTargetView* m_bufferRTVs[1];
        D3D11_VIEWPORT m_viewport;
        
        eastl::unique_ptr<Bind::DepthStencilState> m_depthStencilState;
        eastl::unique_ptr<Bind::PixelShader> m_pixelShader;

        eastl::unique_ptr<Bind::Sampler> m_GBufferSampler;

        // For custom shames/meshes
        eastl::vector<eastl::unique_ptr<SE_G::ColliderTechnique>> m_customTechniques;

        static void InitStaticData(ID3D11Device* device);

        static bool s_staticDataInitializated;
        // For default shapes
        static eastl::unique_ptr<Bind::Topology> s_topology;
        static eastl::unique_ptr<Bind::IndexBuffer> s_shapesIndexBuffer;
        static eastl::unique_ptr<Bind::VertexBuffer> s_shapesVertexBuffer;
        static eastl::unordered_map<SE::ColliderShapeType, SE::ColliderBufferOffset> s_shapeBufferOffsets;

        /*
        struct CamGCB {
            DX::XMMATRIX viewMat;
            DX::XMMATRIX projMat;
            DX::XMFLOAT3 camPos;
            float pad;
        };
        eastl::unique_ptr<Bind::GeometryConstantBuffer<CamGCB>> m_camGCB;

        struct SpritesheetInfoPCB {
            UINT width = 1024u;
            UINT height = 1024u;
            UINT uStep = 128u;
            UINT vStep = 128u;
            UINT uSteps = 8u;
            UINT vSteps = 8u;
        } m_spritesheetData;
        eastl::unique_ptr<Bind::PixelConstantBuffer<SpritesheetInfoPCB>> m_spritesheetInfoPCB;
        */
    };

    class TriggerPass : public ColliderPass
    {
    public:
        TriggerPass(ID3D11Device* device, ID3D11DeviceContext* context,
            eastl::shared_ptr<GBuffer> pGBuffer,
            eastl::shared_ptr<Camera> camera);
    };
}
