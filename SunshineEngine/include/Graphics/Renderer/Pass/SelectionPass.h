#pragma once
#include "RenderPass.h"
#include <Graphics/Renderer/GBuffer.h>
#include <Graphics/Utils/Camera.h>
#include <Utils/UUID.h>
#include <Graphics/Renderer/Pass/IconPass.h>

namespace SE_G {
    class SelectionPass :
        public RenderPass
    {
    public:
        SelectionPass(ID3D11Device* device, ID3D11DeviceContext* context,
            eastl::shared_ptr<GBuffer> pGBuffer,
            eastl::shared_ptr<Camera> camera);
        ~SelectionPass();

        // Inherited via RenderPass
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

        eastl::unique_ptr<Bind::Sampler> m_GBufferSampler;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilWriteMask;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilReadMask;
        SE::UUID m_selectedObjectUUID;

        eastl::unique_ptr<Bind::VertexShader> m_meshVertexShader;

        eastl::unique_ptr<Bind::VertexShader> m_iconVertexShader;
        eastl::unique_ptr<Bind::GeometryShader> m_iconGeometryShader;
        eastl::unique_ptr<Bind::GeometryConstantBuffer<float>> m_selectionBuffer;
        IconPass* m_iconPass;

        eastl::unique_ptr<Bind::PixelShader> m_pixelShader;

        Scene_Info* m_scene;


        /*
        struct ScreenInfoPCB {
            DXSM::Vector2 screenSize;
        } m_screenData;
        Bind::PixelConstantBuffer<ScreenInfoPCB>* m_screenInfoPCB;
        */
    };
}
