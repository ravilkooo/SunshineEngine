#pragma once

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/Technique/RenderTechnique.h>
#include <Graphics/Renderer/GBuffer.h>
//#include <Scene.h>

namespace SE_G {
    class DeferredRenderer :
        public RenderingSystem
    {
    public:
        DeferredRenderer();
        DeferredRenderer(HWND hWnd, UINT screenWidth, UINT screenHeight);

        void InitGBuffer(UINT screenWidth, UINT screenHeight);

        //void RenderScene(const Scene& scene) override;
        void RenderScene() override;
        void AddPass(eastl::shared_ptr<RenderPass> pass) override;
        void AddTechnique(eastl::unique_ptr<RenderTechnique> tech);

        eastl::shared_ptr<GBuffer> pGBuffer;

        void PreResize();
        void OnResize(UINT resizeWidth, UINT resizeHeight);

        DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    };
}
