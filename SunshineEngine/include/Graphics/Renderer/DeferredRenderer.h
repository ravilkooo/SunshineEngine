#pragma once
#include <Windows.h> // UINT

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/unordered_map.h>

#include <Graphics/Renderer/RenderGroup.h>

class ID3D11Device;
class ID3D11DeviceContext;

namespace SE
{
    class ParticleSystem;
}

namespace SE_G
{
    class GBuffer;
    class Camera;

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
        void InitParticleSystem();
        void SetParticleSystem(eastl::shared_ptr<SE::ParticleSystem> ps);

        void SetMainCamera(eastl::shared_ptr<Camera> camera);
        eastl::shared_ptr<Camera> GetMainCamera() override;

        void OnResize(UINT resizeWidth, UINT resizeHeight);

        virtual void Pass() override;

        eastl::shared_ptr<Camera> m_mainCamera;

        eastl::shared_ptr<GBuffer> m_GBuffer;

        UINT m_screenWidth = 800u;
        UINT m_screenHeight = 800u;

        eastl::shared_ptr<SE::ParticleSystem> m_particleSystem;
    };
}
