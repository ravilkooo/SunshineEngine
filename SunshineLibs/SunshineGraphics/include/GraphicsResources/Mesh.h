#pragma once

#include <Resource.h>

#include <memory>
#include <vector>
#include <stdexcept>
#include <d3d11.h>
#include <wrl/client.h>
#include <SimpleMath.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>

using namespace DirectX::SimpleMath;

struct Vertex
{
    Vector3 position;
    Vector4 color;
    Vector2 texcoord;
    Vector3 normal;
};

enum VertexAttributesFlags : UINT {
    POSITION = 0x1,
    COLOR = 0x2,
    UV = 0x4,
    NORMAL = 0x8,
};


class Mesh : public Resource
{
public:
    Mesh() {};
    Mesh(ID3D11Device* device,
        const std::string& path);
    ~Mesh();

    bool LoadModel(std::vector<Vertex>& vertices,
        std::vector<uint32_t>& indices,
        const std::string& path,
        UINT attrFlags = VertexAttributesFlags::POSITION);

    void CreateUnwrappedCubeMesh(
        std::vector<Vertex>& vertices,
        std::vector<uint32_t>& indices);

    void CreateUnwrappedCubeMesh_repeat(
        std::vector<Vertex>& vertices,
        std::vector<uint32_t>& indices);

    void Draw(ID3D11DeviceContext* context) const;
    void Release();

    UINT GetIndexCount() const { return m_indexCount; }

private:
    D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    UINT m_indexCount = 0;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthState;
};

