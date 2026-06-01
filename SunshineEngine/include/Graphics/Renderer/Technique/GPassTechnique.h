#pragma once
#include "RenderTechnique.h"
#include <Utils/UUID.h>

class ID3D11Device;
class ID3D11DeviceContext;
class MeshData;

#include <d3d11.h>


namespace SE_G {
    namespace Bind {
        template <typename T>
        class PixelConstantBuffer;
    }

    class GPassTechnique :
        public RenderTechnique
    {
    public:
        eastl::unique_ptr<Bind::PixelConstantBuffer<SE::UUIDhilo>> m_uuidBuffer;

        GPassTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique,
            SE::UUID uuid);
        ~GPassTechnique();

        // move
        GPassTechnique(GPassTechnique&& other) noexcept;
        GPassTechnique& operator=(GPassTechnique&& other) noexcept;

        void SetRasterizer(D3D11_RASTERIZER_DESC rastDesc);

        void BindAll(ID3D11DeviceContext* context) override;
        void DrawTechnique(ID3D11DeviceContext* context) override;

        // Associate this technique with a MeshData so the mesh can be shared
        // with other systems and modified externally.
        void InitByMeshData(eastl::shared_ptr<MeshData> meshData);
        eastl::shared_ptr<MeshData> m_meshData;

        ID3D11Device* m_device;
    };
}
