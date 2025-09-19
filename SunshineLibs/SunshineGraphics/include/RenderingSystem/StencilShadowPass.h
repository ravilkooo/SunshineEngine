#pragma once



#include "RenderPass.h"

class StencilShadowPass :
    public RenderPass
{

    void StartFrame() override;
    void EndFrame() override;
};

