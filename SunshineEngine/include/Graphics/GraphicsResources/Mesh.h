#pragma once

//#include <IResource.h>

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
#include <EASTL/string.h>

#include <Utils/AssetPath.h>

#include <Graphics/Bindable/VertexBuffer.h>
#include <Graphics/Bindable/IndexBuffer.h>
#include <Graphics/Bindable/Topology.h>

namespace DXSM = DirectX::SimpleMath;
namespace DX = DirectX;

namespace SE_G {
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


    class Mesh
        // : public Resource
    {
    public:
        Mesh() {};
        Mesh(ID3D11Device* device,
            AssetPath meshPath);
        ~Mesh();

        void ClearMesh();

        static bool LoadModel(eastl::vector<Vertex>& vertices,
            eastl::vector<uint32_t>& indices,
            AssetPath meshPath,
            UINT attrFlags = VertexAttributesFlags::POSITION);

        static eastl::shared_ptr<Mesh> CreateUnwrappedBoxMesh(
            ID3D11Device* device,
            DXSM::Vector3 size = DXSM::Vector3::One /* (width, height, length), (x, y, z) */
        );

        static eastl::shared_ptr<Mesh> CreateUnwrappedBoxMesh_repeat(
            ID3D11Device* device,
            DXSM::Vector3 size = DXSM::Vector3::One /* (width, height, length), (x, y, z) */);

        static eastl::shared_ptr<Mesh> CreateSphereMesh(
            ID3D11Device* device,
            DXSM::Vector3 size = DXSM::Vector3::One /* (width, height, length), (x, y, z) */,
            uint32_t sliceCount = 10, uint32_t stackCount = 20);

        static eastl::shared_ptr<Mesh> CreateGeosphereMesh(
            ID3D11Device* device,
            DXSM::Vector3 size = DXSM::Vector3::One /* (width, height, length), (x, y, z) */,
            uint32_t numSubdivisions = 2);

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
            DXSM::Vector3 size = DXSM::Vector3::One /* (width, height, length), (x, y, z) */);

        static void FillUnwrappedBoxMesh_repeat(
            eastl::vector<Vertex>& vertices,
            eastl::vector<uint32_t>& indices,
            DXSM::Vector3 size = DXSM::Vector3::One /* (width, height, length), (x, y, z) */);

        static void FillSphereMesh(
            eastl::vector<Vertex>& vertices,
            eastl::vector<uint32_t>& indices,
            DXSM::Vector3 size = DXSM::Vector3::One /* (width, height, length), (x, y, z) */,
            uint32_t sliceCount = 10, uint32_t stackCount = 10);

        static void FillSphereMesh_old(
            eastl::vector<Vertex>& vertices,
            eastl::vector<uint32_t>& indices,
            DXSM::Vector3 size = DXSM::Vector3::One /* (width, height, length), (x, y, z) */,
            uint32_t sliceCount = 10, uint32_t stackCount = 10);

        static void FillGeosphereMesh(
            eastl::vector<Vertex>& vertices,
            eastl::vector<uint32_t>& indices,
            DXSM::Vector3 size = DXSM::Vector3::One /* (width, height, length), (x, y, z) */,
            uint32_t numSubdivisions = 6u);

        static void FillScreenAlignedQuad(
            eastl::vector<Vertex>& vertices,
            eastl::vector<uint32_t>& indices);

        void Bind(ID3D11DeviceContext* context) const;
        void Draw(ID3D11DeviceContext* context) const;
        void Release();

        AssetPath m_meshPath;
        AssetPath GetCurrentMeshPath();

        //UINT GetIndexCount() const { return m_indexCount; }

    private:
        UINT m_indexCount = 0;
        eastl::unique_ptr<Bind::IndexBuffer> m_indexBuffer;
        eastl::unique_ptr<Bind::VertexBuffer> m_vertexBuffer;
        eastl::unique_ptr<Bind::Topology> m_topology;

        //eastl::string m_path;


    // private cause every mesh should correspond to its key in ResourceManager
        void ChangeMesh(ID3D11Device* device,
            AssetPath meshPath);
    };
}