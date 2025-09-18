#pragma once



#include <d3d11.h>
#include <wrl.h>    
#include <directxmath.h>

#include "RenderingSystem.h"
#include "Windows/DisplayWindow.h"
#include "GBuffer.h"
#include "GraphicsUtils/Scene.h"

class DeferredRenderer :
    public RenderingSystem
{
public:
    DeferredRenderer();
    DeferredRenderer(DisplayWindow* displayWin);

    // Inherited via RenderingSystem
    void RenderScene(const Scene& scene) override;
    void AddPass(RenderPass* pass) override;

    //GBuffer gBuffer;

    std::vector<RenderPass*> passes;
};

