#pragma once

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/unordered_map.h>

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include <Graphics/Renderer/RenderGroup.h>
#include <Graphics/Renderer/Technique/RenderTechnique.h>
#include <Graphics/Renderer/GBuffer.h>
#include <Graphics/Utils/Camera.h>

//#include <Scene.h>

namespace SE_G {
    class DeferredRenderer :
        public RenderGroup
    {
    public:
        DeferredRenderer(
            eastl::string name, ID3D11Device* device, ID3D11DeviceContext* context,
            UINT screenWidth, UINT screenHeight);
        ~DeferredRenderer();

        ID3D11Device* GetDevice() { return m_device; }
        ID3D11DeviceContext* GetDeviceContext() { return m_context; }

        void InitGBuffer(UINT screenWidth, UINT screenHeight);

        void SetMainCamera(eastl::shared_ptr<Camera> camera);
        eastl::shared_ptr<Camera> GetMainCamera();

        void OnResize(UINT resizeWidth, UINT resizeHeight);
        
        virtual void Pass() override;

        eastl::shared_ptr<Camera> m_mainCamera;

        eastl::shared_ptr<GBuffer> m_GBuffer;

        UINT m_screenWidth = 800u;
        UINT m_screenHeight = 800u;

    protected:
        ID3D11Device* m_device;
        ID3D11DeviceContext* m_context;
    };
}
