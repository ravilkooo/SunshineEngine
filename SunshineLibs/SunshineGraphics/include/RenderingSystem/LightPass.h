#pragma once



#include <stdexcept>

#include "RenderPass.h"
#include <GraphicsUtils/Camera.h>
#include "GBuffer.h"
#include <LightObjects/LightCollection.h>

// temp
#include <ParticleSystem/ParticleSystem.h>

class LightPass :
    public RenderPass
{
public:
    LightPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer,
        UINT screenWidth, UINT screenHeight, GBuffer* pGBuffer, Camera* camera);

    void StartFrame() override;
    void Pass(const Scene& scene) override;
    void EndFrame() override;

    Camera* GetCamera();
    void SetCamera(Camera* camera);
    Camera* camera;

    UINT screenWidth = 800;
    UINT screenHeight = 800;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    GBuffer* pGBuffer;

    ID3D11RenderTargetView* gBufferRTV;
    D3D11_VIEWPORT viewport;

    struct CamPCB {
        XMMATRIX viewMatInverse;
        XMMATRIX projMatInverse;
        XMFLOAT3 camPos;
        float pad;
    } cameraData;
    Bind::PixelConstantBuffer<CamPCB>* camPCB;

public:
    std::vector<ParticleSystem*> particleSystems;
    
};

