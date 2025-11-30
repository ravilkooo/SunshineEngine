#pragma once
#include "RenderTechnique.h"
#include <Graphics/Bindable/ConstantBuffer.h>
#include <Utils/UUID.h>
// forward declare MeshData to avoid including MeshComponent.h here
class MeshData;

#include <d3d11.h>


namespace SE_G {
    class GPassTechnique :
        public RenderTechnique
    {
    public:
        eastl::unique_ptr<Bind::PixelConstantBuffer<UUIDhilo>> m_uuidBuffer;

        GPassTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique,
            SE::UUID uuid);
        ~GPassTechnique();

        // move
        GPassTechnique(GPassTechnique&& other) noexcept;
        GPassTechnique& operator=(GPassTechnique&& other) noexcept;

        void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
        void DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        /*
        // To-do: it should:
        // 1) forget about old texture OBJECT
        // 2) Assign NEW texture OBJECT
        void SetTexture(const eastl::wstring& filePath,
            SE_G::Bind::SamplerPreset samplerPreset = SE_G::Bind::SamplerPreset::Wrap);
        */

        /*
        // To-do: it should:
        // 1) forget about old mesh OBJECT
        // 2) Assign NEW mesh OBJECT
        void SetMesh(const eastl::string& filePath);
        void SetMesh(eastl::shared_ptr<SE_G::Mesh> newMesh);
        */

        /*
        // To-do: it should:
        // 1) forget about old texture OBJECT
        // 2) Assign NEW texture OBJECT
        void SetColor(SE_G::Color color);
        */

        // Associate this technique with a MeshData so the mesh can be shared
        // with other systems and modified externally.
        void InitByMeshData(eastl::shared_ptr<MeshData> meshData);
        eastl::shared_ptr<MeshData> m_meshData;

        ID3D11Device* m_device;

        // otherwise textured
        //bool m_colored = true;
    private:
        // To-do: it should:
        // 1) forget about old texture OBJECT
        // 2) Assign NEW texture OBJECT
        //void ClearTexture();
        /*
        void ClearMesh();
        */
    };
}
