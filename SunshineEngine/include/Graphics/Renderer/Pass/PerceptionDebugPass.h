#pragma once
#include "RenderPass.h"
#include <Graphics/Renderer/GBuffer.h>

#include <Graphics/Utils/Camera.h>

#include <Graphics/Bindable/DepthStencilState.h>

namespace SE_G {

    struct PerceptionVertex {
        DXSM::Vector3 position = { 0.0f, 0.0f, 0.0f };
    };

    class PerceptionDebugPass :
        public RenderPass
    {
    public:
        PerceptionDebugPass(ID3D11Device* device, ID3D11DeviceContext* context,
            eastl::shared_ptr<GBuffer> pGBuffer,
            eastl::shared_ptr<Camera> camera);
        ~PerceptionDebugPass();

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

        eastl::unique_ptr<Bind::Topology> m_topology;
        eastl::unique_ptr<Bind::IndexBuffer> m_indexBuffer;
        eastl::unique_ptr<Bind::VertexBuffer> m_vertexBuffer;
    };
}
