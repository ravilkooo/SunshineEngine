#pragma once

#include <d3d11.h>

#include <EASTL/shared_ptr.h>

#include <Graphics/Renderer/Pass/RenderPass.h>

namespace SE_G {
    class GBuffer;
    class Camera;
    class DeferredRenderer;
    
    class GPass :
        public RenderPass
    {
        friend class ShadowMapPass;
    public:
        GPass(DeferredRenderer* renderer,
            eastl::shared_ptr<GBuffer> pGBuffer);
        ~GPass();

        // Inherited via RenderPass
        void StartFrame() override;
        void Pass() override;
        void EndFrame() override;

        void OnResize(UINT resizeWidth, UINT resizeHeight) override;

        UINT screenWidth = 800;
        UINT screenHeight = 800;

        ID3D11RenderTargetView* gBufferRTVs[5];
        D3D11_VIEWPORT viewport;

        eastl::shared_ptr<GBuffer> pGBuffer;
    };
}
