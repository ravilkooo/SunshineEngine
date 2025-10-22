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
        eastl::shared_ptr<GBuffer> pGBuffer,
        eastl::shared_ptr<Camera> camera);

    void StartFrame() override;
    void Pass(const Scene& scene) override;
    void EndFrame() override;

    eastl::shared_ptr<Camera> GetCamera();
    void SetCamera(eastl::shared_ptr<Camera> camera);

    void OnResize(UINT resizeWidth, UINT resizeHeight,
        eastl::shared_ptr<GBuffer> pGBuffer);

    eastl::shared_ptr<Camera> m_camera;

    UINT m_screenWidth = 800u;
    UINT m_screenHeight = 800u;

    eastl::shared_ptr<GBuffer> m_GBuffer;

    D3D11_VIEWPORT m_viewport;

    struct CamPCB {
        DX::XMMATRIX viewMatInverse;
        DX::XMMATRIX projMatInverse;
        DX::XMFLOAT3 camPos;
        float pad;
    } m_cameraData;
    Bind::PixelConstantBuffer<CamPCB>* m_camPCB;

    struct ScreenInfoPCB {
        DXSM::Vector2 screenSize;
    } m_screenData;
    Bind::PixelConstantBuffer<ScreenInfoPCB>* m_screenInfoPCB;

    eastl::shared_ptr<Bind::Texture> m_NormalTexture;
    eastl::shared_ptr<Bind::Texture> m_AlbedoTexture;
    eastl::shared_ptr<Bind::Texture> m_SpecularTexture;
    eastl::shared_ptr<Bind::Texture> m_WorldPosTexture;

public:
    eastl::vector<ParticleSystem*> particleSystems;
    
};

