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

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/vector.h>

#include "Bindable/VertexBuffer.h"
#include "Bindable/IndexBuffer.h"
#include "Bindable/Topology.h"

namespace DXSM = DirectX::SimpleMath;
namespace DX = DirectX;

struct Vertex
{
    DXSM::Vector3 position;
    DXSM::Vector4 color;
    DXSM::Vector2 texcoord;
    DXSM::Vector3 normal;
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

    bool LoadModel(eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices,
        const std::string& path,
        UINT attrFlags = VertexAttributesFlags::POSITION);

    static eastl::shared_ptr<Mesh> CreateUnwrappedBoxMesh(
        ID3D11Device* device,
        float width = 1.0f, float height = 1.0f, float length = 1.0f
        );

    static eastl::shared_ptr<Mesh> CreateUnwrappedBoxMesh_repeat(
        ID3D11Device* device,
        float width = 1.0f, float height = 1.0f, float length = 1.0f);

    static eastl::shared_ptr<Mesh> CreateSphereMesh(
        ID3D11Device* device,
        float radius = 1.0f, uint32_t sliceCount = 10, uint32_t stackCount = 10);

    static eastl::shared_ptr<Mesh> CreateGeosphereMesh(
        ID3D11Device* device,
        float radius = 1.0f,
        UINT numSubdivisions = 6u);

    static eastl::shared_ptr<Mesh> CreateScreenAlignedQuad(
        ID3D11Device* device);

    static Vertex MidPoint(
        const Vertex& v0, const Vertex& v1);

    static void Subdivide(
        eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices);

    static void FillUnwrappedBoxMesh(
        eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices,
        float width = 1.0f, float height = 1.0f, float length = 1.0f);

    static void FillUnwrappedBoxMesh_repeat(
        eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices,
        float width = 1.0f, float height = 1.0f, float length = 1.0f);

    static void FillSphereMesh(
        eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices,
        float radius = 1.0f, uint32_t sliceCount=10, uint32_t stackCount=10);

    static void FillGeosphereMesh(
        eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices,
        float radius = 1.0f,
        UINT numSubdivisions = 6u);

    static void FillScreenAlignedQuad(
        eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices);

    void Draw(ID3D11DeviceContext* context) const;
    void Release();

    //UINT GetIndexCount() const { return m_indexCount; }

private:
    // Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    // Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    
    UINT m_indexCount = 0;
    eastl::unique_ptr<Bind::IndexBuffer> m_indexBuffer;
    eastl::unique_ptr<Bind::VertexBuffer> m_vertexBuffer;
    eastl::unique_ptr<Bind::Topology> m_topology;
};

