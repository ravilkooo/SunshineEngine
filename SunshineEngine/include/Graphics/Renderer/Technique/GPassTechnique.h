#pragma once
#include "RenderTechnique.h"
#include <Graphics/Bindable/ConstantBuffer.h>
#include <Utils/UUID.h>
#include <Component/MeshComponent.h>



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

        // To-do: it should:
        // 1) forget about old texture OBJECT
        // 2) Assign NEW texture OBJECT
        void SetTexture(const eastl::wstring& filePath,
            SE_G::Bind::SamplerPreset samplerPreset = SE_G::Bind::SamplerPreset::Wrap);

        // To-do: it should:
        // 1) forget about old mesh OBJECT
        // 2) Assign NEW mesh OBJECT
        void SetMesh(const eastl::string& filePath);

        // To-do: it should:
        // 1) forget about old texture OBJECT
        // 2) Assign NEW texture OBJECT
        void SetColor(SE_G::Color color);

        void SetMesh(eastl::shared_ptr<SE_G::Mesh> newMesh);

        // Associate this technique with a MeshComponent so the mesh can be shared
        // with other systems and modified externally.
        void SetMeshComponent(MeshComponent* meshComponent);

        DeferredRenderer* m_renderSystem;

        // otherwise textured
        bool m_colored = true;
    private:
        MeshComponent* m_meshComponent = nullptr;
        // To-do: it should:
        // 1) forget about old texture OBJECT
        // 2) Assign NEW texture OBJECT
        void ClearTexture();
        /*
        void ClearMesh();
        */
    };
}
