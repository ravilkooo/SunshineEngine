#pragma once
#include "RenderPass.h"
#include <Graphics/Renderer/GBuffer.h>
#include <Graphics/Utils/Camera.h>
#include <Utils/UUID.h>
#include <GameObject/Lighting/LightCollection.h>

// forward declare bindable resources used by IconPass to reduce includes
namespace SE_G { namespace Bind {
    class VertexShader;
    class GeometryShader;
    class PixelShader;
    class Texture;
    class DepthStencilState;
    class Sampler;
    template<typename> class GeometryConstantBuffer;
    template<typename> class PixelConstantBuffer;
} }

namespace SE_G {
    class IconPass :
        public RenderPass
    {
    public:
        IconPass(ID3D11Device* device, ID3D11DeviceContext* context,
            eastl::shared_ptr<GBuffer> pGBuffer,
            eastl::shared_ptr<Camera> camera);
        ~IconPass();

        // Inherited via RenderPass
        void StartFrame() override;
        void Pass() override;
        void EndFrame() override;

        void OnResize(UINT resizeWidth, UINT resizeHeight) override;
            //eastl::shared_ptr<GBuffer> pGBuffer);

        UINT m_screenWidth = 800u;
        UINT m_screenHeight = 800u;

        eastl::shared_ptr<GBuffer> m_GBuffer;

        ID3D11RenderTargetView* m_bufferRTVs[2];
        D3D11_VIEWPORT m_viewport;

        eastl::shared_ptr<Bind::VertexShader> m_iconVertexShader;
        eastl::unique_ptr<Bind::GeometryShader> m_iconGeometryShader;
        eastl::unique_ptr<Bind::PixelShader> m_iconPixelShader;
        eastl::unique_ptr<Bind::Texture> m_iconSprites;
        eastl::unique_ptr<Bind::DepthStencilState> m_depthStencilState;

        eastl::unique_ptr<Bind::Sampler> m_GBufferSampler;

        struct CamGCB {
            DX::XMMATRIX viewMat;
            DX::XMMATRIX projMat;
            DX::XMFLOAT3 camPos;
            float pad;
        };
        eastl::unique_ptr<Bind::GeometryConstantBuffer<CamGCB>> m_camGCB;

        struct SpritesheetInfoPCB {
            UINT width = 1024u;
            UINT height = 1024u;
            UINT uStep = 128u;
            UINT vStep = 128u;
            UINT uSteps = 8u;
            UINT vSteps = 8u;
        } m_spritesheetData;
        eastl::unique_ptr<Bind::PixelConstantBuffer<SpritesheetInfoPCB>> m_spritesheetInfoPCB;
    };
}
