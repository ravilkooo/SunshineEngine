#pragma once



#include <stdexcept>

#include "RenderPass.h"
#include "GBuffer.h"
#include <GraphicsUtils/Camera.h>

class GPass :
    public RenderPass
{
public:
    GPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer,
        UINT screenWidth, UINT screenHeight);

    // Inherited via RenderPass
    void StartFrame() override;
    void Pass(const Scene& scene) override;
    void EndFrame() override;

    eastl::shared_ptr<Camera> GetCamera();
    void SetCamera(eastl::shared_ptr<Camera> camera);
    eastl::shared_ptr<Camera> camera;

    UINT screenWidth = 800;
    UINT screenHeight = 800;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    ID3D11RenderTargetView* gBufferRTVs[4];
    D3D11_VIEWPORT viewport;
    GBuffer* pGBuffer;
};

