#pragma once
#include "RenderTechnique.h"
#include <Utils/UUID.h>

#include <SimpleMath.h>

namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

namespace SE_G {
    struct IconData {
        UINT uIdx = 0u;
        UINT vIdx = 0u;
        UINT uStepSize = 1u;
        UINT vStepSize = 1u;
        SE::UUIDhilo uuidHilo;
    };

    struct IconVertex {
        DXSM::Vector3 position = { 0.0f, 0.0f, 0.0f };
        DXSM::Vector2 size = { 1.0f, 1.0f };
    };

    namespace Bind {
        class VertexBuffer;

        template <typename T>
        class PixelConstantBuffer;
    }

    class IconTechnique :
        public RenderTechnique
    {

    public:
        IconTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique,
            IconData iconData, float iconSizeX = 0.1f, float iconSizeY = 0.1f);

        void Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
        void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
        void DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

        //eastl::shared_ptr<Bind::GeometryConstantBuffer<DXSM::Vector3>> m_geometryCB;

        IconData m_iconData;
        eastl::shared_ptr<Bind::PixelConstantBuffer<IconData>> m_iconBuffer;

        eastl::unique_ptr<Bind::VertexBuffer> m_vertexBuffer;

        void CreateVertexBuffer(ID3D11Device* device, float iconSizeX, float iconSizeY);
    };
}
