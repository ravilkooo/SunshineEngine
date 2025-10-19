#pragma once



#include <stdexcept>

#include "RenderPass.h"
#include "GBuffer.h"

#include <GraphicsUtils/Camera.h>
#include <Graphics/Lighting/LightCollection.h>

// temp
#include <ParticleSystem/ParticleSystem.h>

class LightPass :
    public RenderPass
{
public:
    LightPass(ID3D11Device* device, ID3D11DeviceContext* context,
        ID3D11Texture2D* backBuffer,
        eastl::shared_ptr<GBuffer> pGBuffer,
        eastl::shared_ptr<Camera> camera);

    void StartFrame() override;
    void Pass(const Scene& scene) override;
    void EndFrame() override;

    eastl::shared_ptr<Camera> GetCamera();
    void SetCamera(eastl::shared_ptr<Camera> camera);

    eastl::shared_ptr<Camera> m_camera;

    UINT m_screenWidth = 800;
    UINT m_screenHeight = 800;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    eastl::shared_ptr<GBuffer> m_GBuffer;

    ID3D11RenderTargetView* m_GBufferRTV;
    D3D11_VIEWPORT m_viewport;

    struct CamPCB {
        XMMATRIX viewMatInverse;
        XMMATRIX projMatInverse;
        XMFLOAT3 camPos;
        float pad;
    } m_cameraData;
    Bind::PixelConstantBuffer<CamPCB>* m_camPCB;

    struct ScreenInfoPCB {
        DXSM::Vector2 screenSize;
    } m_screenData;
    Bind::PixelConstantBuffer<ScreenInfoPCB>* m_screenInfoPCB;

public:
    eastl::vector<ParticleSystem*> particleSystems;
    
};

