#pragma once
#include "RenderTechnique.h"
#include <Graphics/Bindable/ConstantBuffer.h>
#include <Utils/UUID.h>



namespace SE_G {
    class DeferredRenderer;

    class GPassTechnique :
        public RenderTechnique
    {
    public:
        eastl::unique_ptr<Bind::PixelConstantBuffer<UUIDhilo>> m_uuidBuffer;

        GPassTechnique(DeferredRenderer* renderSystem, TransformComponent* assignedTransform, eastl::string technique,
            SE::UUID uuid);
        ~GPassTechnique();

        void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        void SetTexture(const eastl::wstring& filePath,
            SE_G::Bind::SamplerPreset samplerPreset = SE_G::Bind::SamplerPreset::Wrap);
        void SetColor(SE_G::Color color);

        void SetMesh(const eastl::string& filePath);
        void SetMesh(eastl::shared_ptr<SE_G::Mesh> newMesh);

        DeferredRenderer* m_renderSystem;

        // otherwise textured
        bool m_colored = true;
    private:
        void ClearTexture();
        void ClearMesh();
    };
}
