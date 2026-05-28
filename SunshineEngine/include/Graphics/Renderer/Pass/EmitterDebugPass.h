#pragma once
#include "RenderPass.h"

#include <d3d11.h>

namespace SE_G {
    class Camera;
    class GBuffer;
    namespace Bind {
        class DepthStencilState;
        class PixelShader;
        class Sampler;
        class Topology;
        class IndexBuffer;
        class VertexBuffer;
    }

    struct EmitterVertex {
        DXSM::Vector3 position = { 0.0f, 0.0f, 0.0f };
    };

    class EmitterDebugPass :
        public RenderPass
    {
    public:
        EmitterDebugPass(ID3D11Device* device, ID3D11DeviceContext* context,
            eastl::shared_ptr<GBuffer> pGBuffer,
            eastl::shared_ptr<Camera> camera);
        ~EmitterDebugPass();

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
        eastl::shared_ptr<Bind::PixelShader> m_pixelShader;

        eastl::unique_ptr<Bind::Sampler> m_GBufferSampler;

        static void InitStaticData(ID3D11Device* device);

        static bool s_staticDataInitializated;
        
        static eastl::unique_ptr<Bind::Topology> s_topology;
        static eastl::unique_ptr<Bind::IndexBuffer> s_indexBuffer;
        static eastl::unique_ptr<Bind::VertexBuffer> s_vertexBuffer;
    };
}
