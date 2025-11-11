#pragma once
#include "RenderPass.h"
#include <Graphics/Renderer/GBuffer.h>
#include <Graphics/Utils/Camera.h>
#include <Utils/UUID.h>
#include <Graphics/Lighting/LightCollection.h>
#include <Graphics/GraphicsResources/Texture.h>
#include <Graphics/Bindable/DepthStencilState.h>

namespace SE_G {
    class IconPass :
        public RenderPass
    {
    public:
        IconPass(ID3D11Device* device, ID3D11DeviceContext* context,
            eastl::shared_ptr<GBuffer> pGBuffer,
            eastl::shared_ptr<Camera> camera);

        // Inherited via RenderPass
        void StartFrame() override;
        void Pass() override;
        void EndFrame() override;

        eastl::shared_ptr<Camera> GetCamera();
        void SetCamera(eastl::shared_ptr<Camera> camera);

        void OnResize(UINT resizeWidth, UINT resizeHeight,
            eastl::shared_ptr<GBuffer> pGBuffer);

        eastl::shared_ptr<Camera> m_camera;

        UINT m_screenWidth = 800u;
        UINT m_screenHeight = 800u;

        eastl::shared_ptr<GBuffer> m_GBuffer;

        ID3D11RenderTargetView* m_bufferRTVs[2];
        D3D11_VIEWPORT m_viewport;

        eastl::shared_ptr<Bind::VertexShader> m_iconVertexShader;
        eastl::shared_ptr<Bind::GeometryShader> m_iconGeometryShader;
        eastl::shared_ptr<Bind::PixelShader> m_iconPixelShader;
        eastl::shared_ptr<Bind::Texture> m_iconSprites;
        eastl::shared_ptr<Bind::DepthStencilState> m_depthStencilState;

        struct CamGCB {
            DX::XMMATRIX viewMat;
            DX::XMMATRIX projMat;
            DX::XMFLOAT3 camPos;
            float pad;
        };
        eastl::shared_ptr<Bind::GeometryConstantBuffer<CamGCB>> m_camGCB;

        struct SpritesheetInfoPCB {
            UINT width = 1024u;
            UINT height = 1024u;
            UINT uStep = 128u;
            UINT vStep = 128u;
            UINT uSteps = 8u;
            UINT vSteps = 8u;
        } m_spritesheetData;
        eastl::shared_ptr<Bind::PixelConstantBuffer<SpritesheetInfoPCB>> m_spritesheetInfoPCB;
    };
}
