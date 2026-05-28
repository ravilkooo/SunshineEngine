#pragma once

#include <EASTL/shared_ptr.h>

#include <stdexcept>

#include <Graphics/Renderer/Pass/RenderPass.h>

namespace SE_G {
    class GBuffer;
    class Camera;
    
    class GPass :
        public RenderPass
    {
        friend class ShadowMapPass;
    public:
        GPass(ID3D11Device* device, ID3D11DeviceContext* context,
            eastl::shared_ptr<GBuffer> pGBuffer,
            eastl::shared_ptr<Camera> camera);
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
