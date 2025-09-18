#pragma once



#include <stdexcept>
#include <GraphicsUtils/Camera.h>

#include "RenderPass.h"

class MainColorPass :
    public RenderPass
{
public:
    MainColorPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer,
        UINT screenWidth, UINT screenHeight);
    
    void StartFrame() override;
    void EndFrame() override;

    Camera* GetCamera();
    void SetCamera(Camera* camera);
    Camera* camera;
protected:
    UINT screenWidth = 800;
    UINT screenHeight = 800;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    ID3D11RenderTargetView* renderTargetView;
    D3D11_VIEWPORT viewport;
    ID3D11Texture2D* pDepthStencil;
    ID3D11DepthStencilView* pDSV;


};

