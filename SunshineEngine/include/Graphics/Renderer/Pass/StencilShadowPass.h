#pragma once

#include "RenderPass.h"

namespace SE_G {
    class StencilShadowPass :
        public RenderPass
    {

        void StartFrame() override;
        void EndFrame() override;
    };
}
