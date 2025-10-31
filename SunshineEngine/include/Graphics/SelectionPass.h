#pragma once
#include "RenderPass.h"
#include "GBuffer.h"
#include <GraphicsUtils/Camera.h>
#include <Utils/UUID.h>
#include <Graphics/Lighting/LightCollection.h>

class SelectionPass :
    public RenderPass
{
public:
    SelectionPass(ID3D11Device* device, ID3D11DeviceContext* context,
        eastl::shared_ptr<GBuffer> pGBuffer,
        eastl::shared_ptr<Camera> camera);

    // Inherited via RenderPass
    void StartFrame() override;
    void Pass(const Scene& scene) override;
    void EndFrame() override;

    void WriteToStencilStep(const Scene& scene);
    void WriteToBackBufferStep(const Scene& scene);

    eastl::shared_ptr<Camera> GetCamera();
    void SetCamera(eastl::shared_ptr<Camera> camera);

    void OnResize(UINT resizeWidth, UINT resizeHeight,
        eastl::shared_ptr<GBuffer> pGBuffer);

    eastl::shared_ptr<Camera> m_camera;

    UINT m_screenWidth = 800u;
    UINT m_screenHeight = 800u;

    eastl::shared_ptr<GBuffer> m_GBuffer;

    D3D11_VIEWPORT m_viewport;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilWriteMask;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilReadMask;
    Sunshine::UUID m_selectedObjectUUID;

    eastl::shared_ptr<Bind::VertexShader> m_meshVertexShader;
    eastl::shared_ptr<Bind::VertexShader> m_iconVertexShader;
    
    eastl::shared_ptr<Bind::PixelShader> m_pixelShader;

    /*
    struct ScreenInfoPCB {
        DXSM::Vector2 screenSize;
    } m_screenData;
    Bind::PixelConstantBuffer<ScreenInfoPCB>* m_screenInfoPCB;
    */
};

