#pragma once

#include <EASTL/shared_ptr.h>

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include "RenderingSystem.h"
#include "GBuffer.h"
#include "Scene.h"

namespace SE_G {
    class DeferredRenderer :
        public RenderingSystem
    {
    public:
        DeferredRenderer();
        DeferredRenderer(HWND hWnd, UINT screenWidth, UINT screenHeight);

        void InitGBuffer(UINT screenWidth, UINT screenHeight);

        void RenderScene(const Scene& scene) override;
        void AddPass(eastl::shared_ptr<RenderPass> pass) override;

        eastl::shared_ptr<GBuffer> pGBuffer;

        void PreResize();
        void OnResize(UINT resizeWidth, UINT resizeHeight);

        DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    };
}
