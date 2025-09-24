#pragma once



#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <stdexcept>

#include "RenderingSystem.h"
#include "GBuffer.h"
#include "GraphicsUtils/Scene.h"

class DeferredRenderer :
    public RenderingSystem
{
public:
    DeferredRenderer();
    DeferredRenderer(HWND hWnd, UINT screenWidth, UINT screenHeight);

    // Inherited via RenderingSystem
    void RenderScene(const Scene& scene) override;
    void AddPass(RenderPass* pass) override;

    //GBuffer gBuffer;
};

