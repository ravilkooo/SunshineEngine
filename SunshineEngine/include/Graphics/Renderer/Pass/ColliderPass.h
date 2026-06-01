#pragma once
#include <d3d11.h>

#include "RenderPass.h"

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/unordered_map.h>

class ID3D11Device;
class ID3D11DeviceContext;

namespace SE {
    struct ColliderBufferOffset;
    enum class ColliderShapeType;
}

namespace SE_G {
    class Camera;
    class GBuffer;
    class ColliderTechnique;
    class DeferredRenderer;
    namespace Bind {
        class DepthStencilState;
        class PixelShader;
        class Sampler;
        class Topology;
        class IndexBuffer;
        class VertexBuffer;
    }

    class ColliderPass :
        public RenderPass
    {
    public:
        ColliderPass(DeferredRenderer* renderer,
            eastl::shared_ptr<GBuffer> pGBuffer);
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
        eastl::shared_ptr<Bind::PixelShader> m_pixelShader;

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
    };

    class TriggerPass : public ColliderPass
    {
    public:
        TriggerPass(DeferredRenderer* renderer,
            eastl::shared_ptr<GBuffer> pGBuffer);
    };
}
