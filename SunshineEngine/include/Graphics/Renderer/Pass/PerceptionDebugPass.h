#pragma once
#include "RenderPass.h"
#include <Graphics/Renderer/GBuffer.h>

#include <Graphics/Utils/Camera.h>

#include <Graphics/Bindable/DepthStencilState.h>
#include <Graphics/Bindable/ConstantBuffer.h>

class GameObject_Info;

namespace SE_G {
    struct PerceptionSettings
    {
        DXSM::Vector3 EyesOffset = DXSM::Vector3::Zero;
		float padding = 0.0f;

        float SightRadius = 0.0f;
        float LoseRadius = 0.0f;
        float FieldOfView = 90.0f;
        float HearingRadius = 0.0f;
        
        DXSM::Matrix wMatNoLocalInvTranspose = DXSM::Matrix::Identity;
    };

    enum class PerceptionRadius : uint32_t
    {
        SIGHT_RADIUS = 0u,
        LOSE_RADIUS = 1u,
        HEARING_RADIUS = 2u,
        RADIUS_NUM = 3u
    };

    struct PerceptionVertex {
        DXSM::Vector3 position = { 0.0f, 0.0f, 0.0f };
        // PerceptionRadius value;
    };

    class PerceptionDebugPass :
        public RenderPass
    {
    public:
        PerceptionDebugPass(ID3D11Device* device, ID3D11DeviceContext* context,
            eastl::shared_ptr<GBuffer> pGBuffer,
            eastl::shared_ptr<Camera> camera);
        ~PerceptionDebugPass();

        // Inherited via RenderPass
        void StartFrame() override;
        void Pass() override;
        void EndFrame() override;

        void OnResize(UINT resizeWidth, UINT resizeHeight) override;
        
        void InitVertexBuffer(ID3D11Device* device);

        void SetGameObject(GameObject_Info* gameObject);

        UINT m_screenWidth = 800u;
        UINT m_screenHeight = 800u;

        eastl::shared_ptr<GBuffer> m_GBuffer;

        ID3D11RenderTargetView* m_bufferRTVs[1];
        D3D11_VIEWPORT m_viewport;
        
        eastl::unique_ptr<Bind::DepthStencilState> m_depthStencilState;
        eastl::shared_ptr<Bind::VertexShader> m_vertexShader;
        eastl::unique_ptr<Bind::PixelShader> m_pixelShader;

        eastl::unique_ptr<Bind::Sampler> m_GBufferSampler;

        eastl::unique_ptr<Bind::Topology> m_topology;
        eastl::unique_ptr<Bind::IndexBuffer> m_indexBuffer;
        eastl::unique_ptr<Bind::VertexBuffer> m_vertexBuffer;

        PerceptionSettings m_perceptionData;
        eastl::unique_ptr<Bind::VertexConstantBuffer<PerceptionSettings>> m_settingsCB;

        GameObject_Info* m_gameObject = nullptr;

    private:
        static constexpr UINT segments = 32u;
        static constexpr UINT arcSegments = segments / 2u;
        static constexpr float step = DX::XM_2PI / float(segments);
    };
}
