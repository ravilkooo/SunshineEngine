#pragma once

#include <d3d11.h>
#include <wrl.h>    
#include <directxmath.h>
#include "RenderingSystem.h"

#include "Graphics/Renderer/Pass/RenderPass.h"
#include <Scene.h>

namespace SE_G {
    /*
    class ForwardRenderer : public RenderingSystem
    {
        friend class Bindable;
    public:
        ForwardRenderer();
        ForwardRenderer(HWND hWnd, UINT screenWidth, UINT screenHeight);
        ~ForwardRenderer();

        void RenderScene(const Scene& scene);

        void AddPass(eastl::shared_ptr<RenderPass> pass) override;

        void SetMainCamera(eastl::shared_ptr<Camera> camera);
        eastl::shared_ptr<Camera> GetMainCamera();
    };
    */
}
