#pragma once

#include <EASTL/shared_ptr.h>

#include <stdexcept>
#include <Graphics/Utils/Camera.h>
#include <Graphics/Renderer/GBuffer.h>
#include "RenderPass.h"

namespace SE_G {
    class FinalPass :
        public RenderPass
    {
    public:
        FinalPass(ID3D11Device* device, ID3D11DeviceContext* context,
            ID3D11Texture2D* backBuffer,
            eastl::shared_ptr<GBuffer> pGBuffer, eastl::shared_ptr<Camera> camera);

        void StartFrame() override;
        void EndFrame() override;

        eastl::shared_ptr<Camera> GetCamera();
        void SetCamera(eastl::shared_ptr<Camera> camera);
        eastl::shared_ptr<Camera> camera;
    protected:
        UINT screenWidth = 800;
        UINT screenHeight = 800;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

        ID3D11RenderTargetView* renderTargetView;
        D3D11_VIEWPORT viewport;
        ID3D11Texture2D* pDepthStencil;
        ID3D11DepthStencilView* pDSV;


    };
}
