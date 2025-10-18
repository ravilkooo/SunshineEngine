#pragma once

#include <EASTL/shared_ptr.h>

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include "RenderingSystem.h"
#include "GBuffer.h"
#include "Scene.h"

class DeferredRenderer :
    public RenderingSystem
{
public:
    DeferredRenderer();
    DeferredRenderer(HWND hWnd, UINT screenWidth, UINT screenHeight);

    // Inherited via RenderingSystem
    void RenderScene(const Scene& scene) override;
    void AddPass(RenderPass* pass) override;

    eastl::shared_ptr<GBuffer> pGBuffer;
};

