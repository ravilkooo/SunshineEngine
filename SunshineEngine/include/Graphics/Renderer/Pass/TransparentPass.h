#pragma once

#include <d3d11.h>

#include <EASTL/shared_ptr.h>

#include <Utils/UUID.h>
#include <Graphics/Renderer/Pass/RenderPass.h>

namespace SE_G {
    class GBuffer;
    class Camera;
    class DeferredRenderer;
    
    namespace Bind {
        class DepthStencilState;
        class BlendState;
    }

    class TransparentPass :
        public RenderPass
    {
        friend class ShadowMapPass;
    public:
        TransparentPass(DeferredRenderer* renderer,
            eastl::shared_ptr<GBuffer> pGBuffer);
        ~TransparentPass();

        // Inherited via RenderPass
        void StartFrame() override;
        void Pass() override;
        void EndFrame() override;

        void SortObjects();

        void OnResize(UINT resizeWidth, UINT resizeHeight) override;

        UINT screenWidth = 800;
        UINT screenHeight = 800;

        ID3D11RenderTargetView* gBufferRTVs[4];
        D3D11_VIEWPORT viewport;

        eastl::shared_ptr<GBuffer> pGBuffer;
        eastl::unique_ptr<Bind::DepthStencilState> m_depthStencilState;
        eastl::unique_ptr<Bind::BlendState> m_blendState;

        eastl::vector<SE::UUID> m_objectsOrder;
    };
}
