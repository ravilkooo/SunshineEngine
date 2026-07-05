#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <EASTL/shared_ptr.h>

#include <Utils/UUID.h>
#include <Graphics/Renderer/Pass/RenderPass.h>

namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

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

        // RenderTechnique* AddTechnique(SE::UUID uuid, eastl::unique_ptr<RenderTechnique> tech) override;
        // void RemoveTechnique(SE::UUID uuid) override;

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

        struct TransparentPassData
        {
            SE::UUID objectUUID;
            DXSM::Vector3 pos;
		};
        eastl::vector<TransparentPassData> m_objectsOrder;

        bool m_isDirty = true;
    };
}
