#pragma once

#include <stdexcept>
#include <EASTL/unique_ptr.h>

#include "RenderPass.h"

#include <Graphics/Renderer/GBuffer.h>
#include <Graphics/Utils/Camera.h>
#include <Graphics/Bindable/ConstantBuffer.h>

#include <GameObject/Lighting/LightCollection.h>

// temp
#include <Graphics/ParticleSystem/ParticleSystem.h>

namespace SE_G {
    class LightPass :
        public RenderPass
    {
    public:
        LightPass(ID3D11Device* device, ID3D11DeviceContext* context,
            eastl::shared_ptr<GBuffer> pGBuffer,
            eastl::shared_ptr<Camera> camera);
        ~LightPass();

        void StartFrame() override;
        void Pass() override;
        void EndFrame() override;

        eastl::shared_ptr<Camera> GetCamera();
        void SetCamera(eastl::shared_ptr<Camera> camera);

        void OnResize(UINT resizeWidth, UINT resizeHeight) override;
            //eastl::shared_ptr<GBuffer> pGBuffer);

        eastl::shared_ptr<Camera> m_camera;

        UINT m_screenWidth = 800u;
        UINT m_screenHeight = 800u;

        eastl::shared_ptr<GBuffer> m_GBuffer;

        D3D11_VIEWPORT m_viewport;

        eastl::unique_ptr<Bind::BlendState> m_defaultBlendState;
        eastl::unique_ptr<Bind::Sampler> m_GBufferSampler;

        struct CamPCB {
            DX::XMMATRIX viewMatInverse;
            DX::XMMATRIX projMatInverse;
            DX::XMFLOAT3 camPos;
            float pad;
        } m_cameraData;
        eastl::unique_ptr<Bind::PixelConstantBuffer<CamPCB>> m_camPCB;

        struct ScreenInfoPCB {
            DXSM::Vector2 screenSize;
        } m_screenData;
        eastl::unique_ptr<Bind::PixelConstantBuffer<ScreenInfoPCB>> m_screenInfoPCB;

        eastl::shared_ptr<Bind::Texture> m_NormalTexture;
        eastl::shared_ptr<Bind::Texture> m_AlbedoTexture;
        eastl::shared_ptr<Bind::Texture> m_SpecularTexture;
        eastl::shared_ptr<Bind::Texture> m_WorldPosTexture;

    public:
        eastl::vector<ParticleSystem*> particleSystems;

    };
}
