#include <Graphics/GraphicsResources/Mesh.h>

#include <Graphics/Bindable/VertexBuffer.h>
#include <Graphics/Bindable/IndexBuffer.h>
#include <Graphics/Bindable/Topology.h>

#include <Utils/StringUtils.h>

#include <SimpleMath.h>

namespace SE_G {
    Mesh::Mesh() {};

    Mesh::Mesh(ID3D11Device* device,
        AssetPath meshPath) : m_meshPath(meshPath)
    {
        ChangeMesh(device, meshPath);
    }

    Mesh::~Mesh()
    {
        Release();
        m_meshPath.m_assetRelativePath.clear();
    }


    void Mesh::ChangeMesh(ID3D11Device* device,
        AssetPath meshPath) {
        ClearMesh();
        m_meshPath = meshPath;

        eastl::vector<Vertex> vertices;
        eastl::vector<uint32_t> indices;

        UINT attrFlags = VertexAttributesFlags::POSITION | VertexAttributesFlags::UV | VertexAttributesFlags::NORMAL;

        if (meshPath.m_assetRelativePath == L"Box") {
            FillUnwrappedBoxMesh(vertices, indices);
            m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }
        else if (meshPath.m_assetRelativePath == L"Plane") {
            FillPlaneMesh(vertices, indices);
            m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }
        else if (meshPath.m_assetRelativePath == L"Box_repeat") {
            FillUnwrappedBoxMesh_repeat(vertices, indices);
            m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }
        else if (meshPath.m_assetRelativePath == L"Sphere") {
            FillSphereMesh(vertices, indices, meshPath.m_params.asMesh.param1, meshPath.m_params.asMesh.param2);
            m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }
        else if (meshPath.m_assetRelativePath == L"Geosphere") {
            FillGeosphereMesh(vertices, indices, meshPath.m_params.asMesh.param1);
            m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }
        else if (meshPath.m_assetRelativePath == L"Cylinder") {
            FillCylinderMesh(vertices, indices, meshPath.m_params.asMesh.param1);
            m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }
        else if (meshPath.m_assetRelativePath == L"ScreenAlignedQuad") {
            FillScreenAlignedQuad(vertices, indices);
            m_topology = eastl::make_unique<Bind::Topology>(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        }
        else if (!LoadModel(vertices, indices, meshPath, attrFlags)) {
            m_meshPath = AssetPath(eastl::wstring(L"Box_repeat"));
            FillUnwrappedBoxMesh_repeat(vertices, indices);
            m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }
        else
        {
            m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }

        m_indexCount = static_cast<UINT>(indices.size());
        m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
        m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), m_indexCount);
    }

    void Mesh::FillUnwrappedBoxMesh(
        eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices,
        DXSM::Vector3 size)
    {
        // Развёртка "крестом" (UV-карта 3x4 квадрата, каждая грань в своём прямоугольнике)
        // Текстурные координаты (U, V) для каждой из 6 граней
        // Порядок граней: +X, -X, +Y, -Y, +Z, -Z

        // Размер одного квадрата на UV-карте
        const float du = 1.0f / 4.0f;
        const float dv = 1.0f / 3.0f;

        // Координаты центров квадратов для каждой грани (u0, v0)
        struct FaceUV {
            float u0, v0;
        };
        // Крест:       (см. ASCII ниже)
        //        +Y
        //   -X  +Z  +X  -Z
        //        -Y
        FaceUV faceUVs[6] = {
            {2 * du, 1 * dv}, // +X
            {0 * du, 1 * dv}, // -X
            {1 * du, 0 * dv}, // +Y
            {1 * du, 2 * dv}, // -Y
            {3 * du, 1 * dv}, // -Z
            {1 * du, 1 * dv}  // +Z
        };

        // Вспомогательные данные для каждой грани
        struct Face {
            DXSM::Vector3 normal;
            DXSM::Vector3 v0, v1, v2, v3; // 4 вершины (по часовой стрелке)
            int faceIndex;
        };

        Face faces[] = {
            // +X
            { { 1, 0, 0 }, { 0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f,  0.5f }, { 0.5f,  0.5f,  0.5f }, { 0.5f,  0.5f, -0.5f }, 0 },
            // -X
            { {-1, 0, 0 }, {-0.5f, -0.5f,  0.5f }, {-0.5f, -0.5f, -0.5f }, {-0.5f,  0.5f, -0.5f }, {-0.5f,  0.5f,  0.5f }, 1 },
            // +Y
            { { 0, 1, 0 }, {-0.5f,  0.5f, -0.5f }, { 0.5f,  0.5f, -0.5f }, { 0.5f,  0.5f,  0.5f }, {-0.5f,  0.5f,  0.5f }, 2 },
            // -Y
            { { 0,-1, 0 }, {-0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f, -0.5f }, {-0.5f, -0.5f, -0.5f }, 3 },
            // +Z
            { { 0, 0, 1 }, { 0.5f, -0.5f,  0.5f }, {-0.5f, -0.5f,  0.5f }, {-0.5f,  0.5f,  0.5f }, { 0.5f,  0.5f,  0.5f }, 4 },
            // -Z
            { { 0, 0,-1 }, {-0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f,  0.5f, -0.5f }, {-0.5f,  0.5f, -0.5f }, 5 }
        };

        // Для каждой грани создаём 4 вершины с уникальными UV
        for (int f = 0; f < 6; ++f)
        {
            auto uv = faceUVs[f];
            // 4 угла квадрата в UV (по часовой стрелке)
            float u[4] = { uv.u0,           uv.u0 + du,     uv.u0 + du,     uv.u0 };
            float v[4] = { uv.v0 + dv,      uv.v0 + dv,     uv.v0,          uv.v0 };

            // 4 вершины
            DXSM::Vector3 pos[4] = { faces[f].v0, faces[f].v1, faces[f].v2, faces[f].v3 };
            for (int i = 0; i < 4; ++i)
            {
                Vertex vert;
                vert.position.x = pos[i].x * size.x;
                vert.position.y = pos[i].y * size.y;
                vert.position.z = pos[i].z * size.z;
                vert.normal.x = faces[f].normal.x;
                vert.normal.y = faces[f].normal.y;
                vert.normal.z = faces[f].normal.z;
                vert.texcoord.x = u[i];
                vert.texcoord.y = v[i];
                vertices.push_back(vert);
            }
            // Индексы двух треугольников для этой грани
            int base = f * 4;
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 1);
            indices.push_back(base + 0);
            indices.push_back(base + 3);
            indices.push_back(base + 2);
        }
    }

    void Mesh::FillPlaneMesh(eastl::vector<Vertex>& vertices, eastl::vector<uint32_t>& indices)
    {
        // Плоскость в XZ, нормаль смотрит в +Y
        Vertex v0 = { {-0.5f, 0, -0.5f}, {1, 1, 1, 1}, {0, 1}, {0, 1, 0} };
        Vertex v1 = { { 0.5f, 0, -0.5f}, {1, 1, 1, 1}, {1, 1}, {0, 1, 0} };
        Vertex v2 = { { 0.5f, 0,  0.5f}, {1, 1, 1, 1}, {1, 0}, {0, 1, 0} };
        Vertex v3 = { {-0.5f, 0,  0.5f}, {1, 1, 1, 1}, {0, 0}, {0, 1, 0} };
        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(1);
        indices.push_back(0);
        indices.push_back(3);
		indices.push_back(2);
    }

    void Mesh::FillUnwrappedBoxMesh_repeat(eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices)
    {
        // Развёртка "крестом" (UV-карта 3x4 квадрата, каждая грань в своём прямоугольнике)
        // Текстурные координаты (U, V) для каждой из 6 граней
        // Порядок граней: +X, -X, +Y, -Y, +Z, -Z


        // Вспомогательные данные для каждой грани
        struct Face {
            DXSM::Vector3 normal;
            DXSM::Vector3 v0, v1, v2, v3; // 4 вершины (по часовой стрелке)
            int faceIndex;
        };

        Face faces[] = {
            // +X
            { { 1, 0, 0 }, { 0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f,  0.5f }, { 0.5f,  0.5f,  0.5f }, { 0.5f,  0.5f, -0.5f }, 0 },
            // -X
            { {-1, 0, 0 }, {-0.5f, -0.5f,  0.5f }, {-0.5f, -0.5f, -0.5f }, {-0.5f,  0.5f, -0.5f }, {-0.5f,  0.5f,  0.5f }, 1 },
            // +Y
            { { 0, 1, 0 }, {-0.5f,  0.5f, -0.5f }, { 0.5f,  0.5f, -0.5f }, { 0.5f,  0.5f,  0.5f }, {-0.5f,  0.5f,  0.5f }, 2 },
            // -Y
            { { 0,-1, 0 }, {-0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f, -0.5f }, {-0.5f, -0.5f, -0.5f }, 3 },
            // +Z
            { { 0, 0, 1 }, { 0.5f, -0.5f,  0.5f }, {-0.5f, -0.5f,  0.5f }, {-0.5f,  0.5f,  0.5f }, { 0.5f,  0.5f,  0.5f }, 4 },
            // -Z
            { { 0, 0,-1 }, {-0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f,  0.5f, -0.5f }, {-0.5f,  0.5f, -0.5f }, 5 }
        };

        // Для каждой грани создаём 4 вершины с уникальными UV
        for (int f = 0; f < 6; ++f)
        {
            // 4 угла квадрата в UV (по часовой стрелке)
            float u[4] = { 0, 1, 1, 0 };
            float v[4] = { 1, 1, 0, 0 };

            // 4 вершины
            DX::XMFLOAT3 pos[4] = { faces[f].v0, faces[f].v1, faces[f].v2, faces[f].v3 };
            for (int i = 0; i < 4; ++i)
            {
                Vertex vert;
                vert.position.x = pos[i].x;
                vert.position.y = pos[i].y;
                vert.position.z = pos[i].z;
                vert.normal.x = faces[f].normal.x;
                vert.normal.y = faces[f].normal.y;
                vert.normal.z = faces[f].normal.z;
                vert.texcoord.x = u[i];
                vert.texcoord.y = v[i];
                vertices.push_back(vert);
            }
            // Индексы двух треугольников для этой грани
            int base = f * 4;
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 1);
            indices.push_back(base + 0);
            indices.push_back(base + 3);
            indices.push_back(base + 2);
        }
    }

    void Mesh::FillSphereMesh(
        eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices,
        uint32_t sliceCount, uint32_t stackCount)
    {
        sliceCount = eastl::max(sliceCount, 4u);
        stackCount = eastl::max(stackCount, 1u);

        vertices.resize(2 * sliceCount + (2 * stackCount + 1) * (sliceCount + 1));
        indices.resize(6 * (sliceCount + 1) + 2 * 6 * stackCount * (sliceCount + 1));

        float sliceStep = DX::XM_2PI / sliceCount;
        float elevationStep = DX::XM_PIDIV2 / (stackCount + 1);

        UINT _offsetCommonVertexIdx = 0;
        // top vertex (sliceCount times)
        for (UINT j = 0; j < sliceCount; ++j) {
            vertices[_offsetCommonVertexIdx++] = {
                DXSM::Vector3(0.0f, 1.0f, 0.0f), DXSM::Vector4::One, DXSM::Vector2((2 * j + 1) * 0.5f / sliceCount, 0), DXSM::Vector3(0,1,0)
            };
        }

        // other vertices
        for (UINT i = 1; i <= 2 * stackCount + 1; ++i)
        {
            for (UINT j = 0; j <= sliceCount; ++j) {
                vertices[_offsetCommonVertexIdx++] =
                { DXSM::Vector3(
                    sinf(elevationStep * i) * cosf(sliceStep * j),
                    cosf(elevationStep * i),
                    sinf(elevationStep * i) * sinf(sliceStep * j)
                ),
                    DXSM::Vector4::One,
                    DXSM::Vector2(j * 1.0f / sliceCount, (i * 1.0f) / (2 * stackCount + 2)),
                    DXSM::Vector3(
                        sinf(elevationStep * i) * cosf(sliceStep * j),
                        cosf(elevationStep * i),
                        sinf(elevationStep * i) * sinf(sliceStep * j)
                    ) };
            }
        }
        // bottom vertex (sliceCount times)
        for (UINT j = 0; j < sliceCount; ++j) {
            vertices[_offsetCommonVertexIdx++] = {
                DXSM::Vector3(0.0f, -1.0f, 0.0f), DXSM::Vector4::One, DXSM::Vector2((2 * j + 1) * 0.5f / sliceCount, 1),
                DXSM::Vector3(0,-1,0) };
        }

        UINT indexIndex = 0;

        for (UINT j = 0; j < sliceCount; ++j) {
            indices[indexIndex++] = j;
            indices[indexIndex++] = j + sliceCount + 1;
            indices[indexIndex++] = j + sliceCount;
        }


        for (UINT i = 0; i < 2 * stackCount; ++i) {
            UINT startIndex = sliceCount /* change (1) to (sliceCount) duplicate top */ + i * (sliceCount + 1);
            UINT nextStartIndex = startIndex + (sliceCount + 1);
            for (UINT j = 0; j < sliceCount; ++j) {

                indices[indexIndex++] = startIndex + j;
                indices[indexIndex++] = startIndex + j + 1;
                indices[indexIndex++] = nextStartIndex + j;

                indices[indexIndex++] = startIndex + j + 1;
                indices[indexIndex++] = nextStartIndex + j + 1;
                indices[indexIndex++] = nextStartIndex + j;
            }

            indices[indexIndex++] = startIndex + sliceCount;
            indices[indexIndex++] = startIndex;
            indices[indexIndex++] = nextStartIndex + sliceCount;

            indices[indexIndex++] = startIndex;
            indices[indexIndex++] = nextStartIndex;
            indices[indexIndex++] = nextStartIndex + sliceCount;
        }


        UINT bottomIndex = _offsetCommonVertexIdx - sliceCount /* change (1) to (sliceCount) duplicate top */;
        UINT startIndex = sliceCount /* change (1) to (sliceCount) duplicate top */ + 2 * stackCount * (sliceCount + 1);
        for (UINT j = 0; j < sliceCount; ++j) {
            indices[indexIndex++] = bottomIndex + j;
            indices[indexIndex++] = startIndex + j;
            indices[indexIndex++] = startIndex + j + 1;
        }
        /*
        indices[indexIndex++] = _offsetCommonVertexIdx - 1;
        indices[indexIndex++] = startIndex + sliceCount;
        indices[indexIndex++] = startIndex;
        */

        return;
    }

    void Mesh::FillSphereMesh_old(
        eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices,
        uint32_t sliceCount, uint32_t stackCount)
    {
        vertices.resize(0);
        indices.resize(0);

        Vertex topVertex(
            DXSM::Vector3(0.0f, +1.0f, 0.0f),
            DXSM::Vector4::One,
            DXSM::Vector2(0.0f, 0.0f),
            DXSM::Vector3(0.0f, +1.0f, 0.0f));

        Vertex bottomVertex(
            DXSM::Vector3(0.0f, -1.0f, 0.0f),
            DXSM::Vector4::One,
            DXSM::Vector2(0.0f, 1.0f),
            DXSM::Vector3(0.0f, -1.0f, 0.0f));

        vertices.push_back(topVertex);

        float phiStep = DX::XM_PI / stackCount;
        float thetaStep = 2.0f * DX::XM_PI / sliceCount;

        for (uint32_t i = 1; i <= stackCount - 1; ++i)
        {
            float phi = i * phiStep;

            for (uint32_t j = 0; j <= sliceCount; ++j)
            {
                float theta = j * thetaStep;

                Vertex v;

                v.position.x = sinf(phi) * cosf(theta);
                v.position.y = cosf(phi);
                v.position.z = sinf(phi) * sinf(theta);

                v.color = DXSM::Vector4::One;
                v.normal = v.position;
                v.normal.Normalize();

                v.texcoord.x = theta / DX::XM_2PI;
                v.texcoord.y = phi / DX::XM_PI;

                vertices.push_back(v);
            }
        }

        vertices.push_back(bottomVertex);


        for (uint32_t i = 1; i <= sliceCount; ++i)
        {
            indices.push_back(0);
            indices.push_back(i + 1);
            indices.push_back(i);
        }

        uint32_t baseIndex = 1;
        uint32_t ringVertexCount = sliceCount + 1;
        for (uint32_t i = 0; i < stackCount - 2; ++i)
        {
            for (uint32_t j = 0; j < sliceCount; ++j)
            {
                indices.push_back(baseIndex + i * ringVertexCount + j);
                indices.push_back(baseIndex + i * ringVertexCount + j + 1);
                indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

                indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
                indices.push_back(baseIndex + i * ringVertexCount + j + 1);
                indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
            }
        }

        uint32_t southPoleIndex = (uint32_t)vertices.size() - 1;

        baseIndex = southPoleIndex - ringVertexCount;

        for (uint32_t i = 0; i < sliceCount; ++i)
        {
            indices.push_back(southPoleIndex);
            indices.push_back(baseIndex + i);
            indices.push_back(baseIndex + i + 1);
        }

        return;
    }

    void Mesh::FillGeosphereMesh(eastl::vector<Vertex>& vertices,
        eastl::vector<UINT>& indices,
        uint32_t numSubdivisions)
    {
        numSubdivisions = std::min<uint32_t>(std::max<uint32_t>(numSubdivisions, 0), 5);

        const float X = 0.525731f;
        const float Z = 0.850651f;

        /*
        DXSM::Vector3 pos[60]
        {
            DXSM::Vector3(X, 0.0f, Z), DXSM::Vector3(0.0f, Z, X), DXSM::Vector3(-X, 0.0f, Z),
            DXSM::Vector3(0.0f, Z, X), DXSM::Vector3(-Z, X, 0.0f), DXSM::Vector3(-X, 0.0f, Z),
            DXSM::Vector3(0.0f, Z, X), DXSM::Vector3(0.0f, Z, -X), DXSM::Vector3(-Z, X, 0.0f),
            DXSM::Vector3(Z, X, 0.0f), DXSM::Vector3(0.0f, Z, -X), DXSM::Vector3(0.0f, Z, X),
            DXSM::Vector3(X, 0.0f, Z), DXSM::Vector3(Z, X, 0.0f), DXSM::Vector3(0.0f, Z, X),
            DXSM::Vector3(X, 0.0f, Z), DXSM::Vector3(Z, -X, 0.0f), DXSM::Vector3(Z, X, 0.0f),
            DXSM::Vector3(Z, -X, 0.0f), DXSM::Vector3(X, 0.0f, -Z), DXSM::Vector3(Z, X, 0.0f),
            DXSM::Vector3(Z, X, 0.0f), DXSM::Vector3(X, 0.0f, -Z), DXSM::Vector3(0.0f, Z, -X),
            DXSM::Vector3(X, 0.0f, -Z), DXSM::Vector3(-X, 0.0f, -Z), DXSM::Vector3(0.0f, Z, -X),
            DXSM::Vector3(X, 0.0f, -Z), DXSM::Vector3(0.0f, -Z, -X), DXSM::Vector3(-X, 0.0f, -Z),
            DXSM::Vector3(X, 0.0f, -Z), DXSM::Vector3(Z, -X, 0.0f), DXSM::Vector3(0.0f, -Z, -X),
            DXSM::Vector3(Z, -X, 0.0f), DXSM::Vector3(0.0f, -Z, X), DXSM::Vector3(0.0f, -Z, -X),
            DXSM::Vector3(0.0f, -Z, X), DXSM::Vector3(-Z, -X, 0.0f), DXSM::Vector3(0.0f, -Z, -X),
            DXSM::Vector3(0.0f, -Z, X), DXSM::Vector3(-X, 0.0f, Z), DXSM::Vector3(-Z, -X, 0.0f),
            DXSM::Vector3(0.0f, -Z, X), DXSM::Vector3(X, 0.0f, Z), DXSM::Vector3(-X, 0.0f, Z),
            DXSM::Vector3(Z, -X, 0.0f), DXSM::Vector3(X, 0.0f, Z), DXSM::Vector3(0.0f, -Z, X),
            DXSM::Vector3(-Z, -X, 0.0f), DXSM::Vector3(-X, 0.0f, Z), DXSM::Vector3(-Z, X, 0.0f),
            DXSM::Vector3(-X, 0.0f, -Z), DXSM::Vector3(-Z, -X, 0.0f), DXSM::Vector3(-Z, X, 0.0f),
            DXSM::Vector3(0.0f, Z, -X), DXSM::Vector3(-X, 0.0f, -Z), DXSM::Vector3(-Z, X, 0.0f),
            DXSM::Vector3(-Z, -X, 0.0f), DXSM::Vector3(-X, 0.0f, -Z), DXSM::Vector3(0.0f, -Z, -X),
        };
        */

        DXSM::Vector3 pos[12] =
        {
            DXSM::Vector3(-X, 0.0f, Z), DXSM::Vector3(X, 0.0f, Z),
            DXSM::Vector3(-X, 0.0f, -Z), DXSM::Vector3(X, 0.0f, -Z),
            DXSM::Vector3(0.0f, Z, X), DXSM::Vector3(0.0f, Z, -X),
            DXSM::Vector3(0.0f, -Z, X), DXSM::Vector3(0.0f, -Z, -X),
            DXSM::Vector3(Z, X, 0.0f), DXSM::Vector3(-Z, X, 0.0f),
            DXSM::Vector3(Z, -X, 0.0f), DXSM::Vector3(-Z, -X, 0.0f)
        };
        DWORD k[60] =
        {
        1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
        1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
        3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
        10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
        };


        vertices.resize(12);
        for (size_t i = 0; i < 12; i++) {
            vertices[i] = { pos[i], DXSM::Vector4(92, 92, 92, 255), DXSM::Vector2::Zero, DXSM::Vector3(1, 0, 0) };
        }

        indices.resize(60);
        for (size_t i = 0; i < 60; ++i)
            indices[i] = k[i];

        for (UINT i = 0; i < numSubdivisions; ++i)
            Subdivide(vertices, indices);

        for (UINT i = 0; i < vertices.size(); ++i)
        {
            vertices[i].normal = vertices[i].position;
            vertices[i].normal.Normalize();

            // [-pi, pi]
            float theta = std::atan2f(vertices[i].normal.x, vertices[i].normal.z);

            // Put in [0, 2pi].
            theta += DX::XM_PI;

            float phi = acosf(vertices[i].normal.y);

            // [0,1]
            vertices[i].texcoord.x = theta / DX::XM_2PI;
            vertices[i].texcoord.y = phi / DX::XM_PI;

            vertices[i].position.x = vertices[i].normal.x;
            vertices[i].position.y = vertices[i].normal.y;
            vertices[i].position.z = vertices[i].normal.z;

            vertices[i].normal = vertices[i].position;
            vertices[i].normal.Normalize();
        }
        return;
    }

    void Mesh::FillCylinderMesh(
        eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices,
        uint32_t sliceCount)
    {
        vertices.clear();
        indices.clear();

        const float halfHeight = 0.5f;

        const float PI = DirectX::XM_PI;
        const float dTheta = 2.0f * PI / static_cast<float>(sliceCount);

        for (uint32_t i = 0; i <= sliceCount; ++i)
        {
            float theta = i * dTheta;

            float c = cosf(theta);
            float s = sinf(theta);

            DXSM::Vector3 normal(c, 0.0f, s);

            float u = static_cast<float>(i) / static_cast<float>(sliceCount);

            {
                Vertex v{};

                v.position = DXSM::Vector3(
                    c,
                    halfHeight,
                    s);

                v.normal = normal;

                v.texcoord = DXSM::Vector2(
                    u,
                    0.0f);

                vertices.push_back(v);
            }

            {
                Vertex v{};

                v.position = DXSM::Vector3(
                    c,
                    -halfHeight,
                    s);

                v.normal = normal;

                v.texcoord = DXSM::Vector2(
                    u,
                    0.5f);

                vertices.push_back(v);
            }
        }

        for (uint32_t i = 0; i < sliceCount; ++i)
        {
            uint32_t i0 = i * 2 + 0;
            uint32_t i1 = i * 2 + 1;
            uint32_t i2 = i * 2 + 2;
            uint32_t i3 = i * 2 + 3;

            // Triangle 1
            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i1);

            // Triangle 2
            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        }

        uint32_t topCenterIndex = static_cast<uint32_t>(vertices.size());

        {
            Vertex center{};

            center.position = DXSM::Vector3(0.0f, halfHeight, 0.0f);
            center.normal = DXSM::Vector3(0.0f, 1.0f, 0.0f);

            center.texcoord = DXSM::Vector2(0.25f, 0.75f);

            vertices.push_back(center);
        }

        for (uint32_t i = 0; i <= sliceCount; ++i)
        {
            float theta = i * dTheta;

            float c = cosf(theta);
            float s = sinf(theta);

            Vertex v{};

            v.position = DXSM::Vector3(
                c,
                halfHeight,
                s);

            v.normal = DXSM::Vector3(0.0f, 1.0f, 0.0f);

            float localU = c * 0.5f;
            float localV = s * 0.5f;

            v.texcoord.x = 0.25f + localU * 0.5f;
            v.texcoord.y = 0.75f - localV * 0.5f;

            vertices.push_back(v);
        }

        for (uint32_t i = 0; i < sliceCount; ++i)
        {
            uint32_t center = topCenterIndex;
            uint32_t v0 = topCenterIndex + 1 + i;
            uint32_t v1 = topCenterIndex + 1 + i + 1;

            indices.push_back(center);
            indices.push_back(v1);
            indices.push_back(v0);
        }

        uint32_t bottomCenterIndex = static_cast<uint32_t>(vertices.size());

        {
            Vertex center{};

            center.position = DXSM::Vector3(0.0f, -halfHeight, 0.0f);
            center.normal = DXSM::Vector3(0.0f, -1.0f, 0.0f);

            center.texcoord = DXSM::Vector2(0.75f, 0.75f);

            vertices.push_back(center);
        }

        for (uint32_t i = 0; i <= sliceCount; ++i)
        {
            float theta = i * dTheta;

            float c = cosf(theta);
            float s = sinf(theta);

            Vertex v{};

            v.position = DXSM::Vector3(
                c,
                -halfHeight,
                s);

            v.normal = DXSM::Vector3(0.0f, -1.0f, 0.0f);

            float localU = c * 0.5f;
            float localV = s * 0.5f;

            v.texcoord.x = 0.75f + localU * 0.5f;
            v.texcoord.y = 0.75f - localV * 0.5f;

            vertices.push_back(v);
        }

        for (uint32_t i = 0; i < sliceCount; ++i)
        {
            uint32_t center = bottomCenterIndex;
            uint32_t v0 = bottomCenterIndex + 1 + i;
            uint32_t v1 = bottomCenterIndex + 1 + i + 1;

            indices.push_back(center);
            indices.push_back(v0);
            indices.push_back(v1);
        }
    }

    void Mesh::Subdivide(eastl::vector<Vertex>& vertices, eastl::vector<uint32_t>& indices)
    {
        // Save a copy of the input geometry.
        eastl::vector<Vertex> vertices_copy;
        eastl::copy(vertices.begin(), vertices.end(), eastl::back_inserter(vertices_copy));

        eastl::vector<uint32_t> indices_copy;
        eastl::copy(indices.begin(), indices.end(), eastl::back_inserter(indices_copy));


        vertices.resize(0);
        indices.resize(0);

        //       v1
        //       *
        //      / \
    	//     /   \
    	//  m0*-----*m1
        //   / \   / \
    	//  /   \ /   \
    	// *-----*-----*
        // v0    m2     v2

        uint32_t numTris = (uint32_t)indices_copy.size() / 3;
        for (uint32_t i = 0; i < numTris; ++i)
        {
            Vertex v0 = vertices_copy[indices_copy[i * 3 + 0]];
            Vertex v1 = vertices_copy[indices_copy[i * 3 + 1]];
            Vertex v2 = vertices_copy[indices_copy[i * 3 + 2]];

            //
            // Generate the midpoints.
            //

            Vertex m0 = MidPoint(v0, v1);
            Vertex m1 = MidPoint(v1, v2);
            Vertex m2 = MidPoint(v0, v2);

            //
            // Add new geometry.
            //

            vertices.push_back(v0); // 0
            vertices.push_back(v1); // 1
            vertices.push_back(v2); // 2
            vertices.push_back(m0); // 3
            vertices.push_back(m1); // 4
            vertices.push_back(m2); // 5

            indices.push_back(i * 6 + 0);
            indices.push_back(i * 6 + 3);
            indices.push_back(i * 6 + 5);

            indices.push_back(i * 6 + 3);
            indices.push_back(i * 6 + 4);
            indices.push_back(i * 6 + 5);

            indices.push_back(i * 6 + 5);
            indices.push_back(i * 6 + 4);
            indices.push_back(i * 6 + 2);

            indices.push_back(i * 6 + 3);
            indices.push_back(i * 6 + 1);
            indices.push_back(i * 6 + 4);
        }
    }

    Vertex Mesh::MidPoint(const Vertex& v0, const Vertex& v1)
    {
        DXSM::Vector3 p0 = v0.position;
        DXSM::Vector3 p1 = v1.position;

        DXSM::Vector4 col0 = v0.color;
        DXSM::Vector4 col1 = v1.color;

        DXSM::Vector2 tex0 = v0.texcoord;
        DXSM::Vector2 tex1 = v1.texcoord;

        DXSM::Vector3 n0 = v0.normal;
        DXSM::Vector3 n1 = v1.normal;


        // Compute the midpoints of all the attributes.  Vectors need to be normalized
        // since linear interpolating can make them not unit length.  
        DXSM::Vector3 pos = 0.5f * (p0 + p1);
        DXSM::Vector4 color = (0.5f * (col0 + col1));
        DXSM::Vector2 tex = 0.5f * (tex0 + tex1);
        DXSM::Vector3 normal = (0.5f * (n0 + n1));
        normal.Normalize();

        return Vertex(pos, color, tex, normal);
    }

    void Mesh::FillScreenAlignedQuad(eastl::vector<Vertex>& vertices, eastl::vector<uint32_t>& indices)
    {
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(3);

        vertices.reserve(0);
    }

    bool Mesh::LoadModel(eastl::vector<Vertex>& vertices,
        eastl::vector<uint32_t>& indices,
        AssetPath meshPath, UINT attrFlags)
    {

        Assimp::Importer importer;
        const aiScene* pModel = importer.ReadFile(WStringToUtf8(meshPath.GetFullPath()).c_str(),
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder
            | (((attrFlags & VertexAttributesFlags::NORMAL) != 0) ? aiProcess_GenNormals : 0x0)
        );

        //	aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
        // aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_MakeLeftHanded


        if (!pModel || pModel->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pModel->mRootNode)
        {
            return false;
        }
        else
        {
            unsigned int meshesNum = 1;

            size_t verticesNum = 0;
            size_t indicesNum = 0;

            verticesNum += pModel->mMeshes[0]->mNumVertices;
            indicesNum += pModel->mMeshes[0]->mNumFaces * 3;

            vertices.resize(verticesNum);
            indices.resize(indicesNum);

            size_t vertexIdx = 0;
            size_t indexIdx = 0;

            const auto pMesh = pModel->mMeshes[0];

            for (unsigned i = 0; i < pMesh->mNumVertices; i++)
            {
                vertices[vertexIdx++] = {
                    DX::XMFLOAT3(
                    pMesh->mVertices[i].x,
                    pMesh->mVertices[i].y,
                    pMesh->mVertices[i].z
                    ) };

                if (pMesh->mTextureCoords[0] && (attrFlags & VertexAttributesFlags::UV))
                {
                    vertices[vertexIdx - 1].texcoord.x = (float)pMesh->mTextureCoords[0][i].x;
                    vertices[vertexIdx - 1].texcoord.y = (float)pMesh->mTextureCoords[0][i].y;
                }

                if (attrFlags & VertexAttributesFlags::NORMAL) {
                    vertices[vertexIdx - 1].normal = {
                        DX::XMFLOAT3(
                        pMesh->mNormals[i].x,
                        pMesh->mNormals[i].y,
                        pMesh->mNormals[i].z
                        ) };
                }
            }
            for (unsigned i = 0; i < pMesh->mNumFaces; i++)
            {
                aiFace face = pMesh->mFaces[i];
                assert(face.mNumIndices == 3);
                for (unsigned j = 0; j < face.mNumIndices; j++) {
                    indices[indexIdx++] = face.mIndices[j];
                }
            }
        }
        return true;
    }

    eastl::shared_ptr<Mesh> Mesh::CreateUnwrappedBoxMesh(
        ID3D11Device* device)
    {
        eastl::shared_ptr<Mesh> mesh = eastl::make_shared<Mesh>();
        eastl::vector<Vertex> vertices;
        eastl::vector<uint32_t> indices;

        FillUnwrappedBoxMesh(vertices, indices);
        mesh->m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mesh->m_meshPath = AssetPath(eastl::wstring(L"Box"));
        mesh->m_indexCount = static_cast<UINT>(indices.size());
        mesh->m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
        mesh->m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), mesh->m_indexCount);
        return mesh;
    }

    eastl::shared_ptr<Mesh> Mesh::CreatePlaneMesh(ID3D11Device* device)
    {
        eastl::shared_ptr<Mesh> mesh = eastl::make_shared<Mesh>();
        eastl::vector<Vertex> vertices;
        eastl::vector<uint32_t> indices;

        FillPlaneMesh(vertices, indices);
        mesh->m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mesh->m_meshPath = AssetPath(eastl::wstring(L"Plane"));
        mesh->m_indexCount = static_cast<UINT>(indices.size());
        mesh->m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
        mesh->m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), mesh->m_indexCount);
        return mesh;
    }

    eastl::shared_ptr<Mesh> Mesh::CreateUnwrappedBoxMesh_repeat(
        ID3D11Device* device)
    {
        eastl::shared_ptr<Mesh> mesh = eastl::make_shared<Mesh>();
        eastl::vector<Vertex> vertices;
        eastl::vector<uint32_t> indices;

        FillUnwrappedBoxMesh_repeat(vertices, indices);
        mesh->m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        mesh->m_meshPath = AssetPath(eastl::wstring(L"Box_repeat"));
        mesh->m_indexCount = static_cast<UINT>(indices.size());
        mesh->m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
        mesh->m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), mesh->m_indexCount);

        return mesh;
    }

    eastl::shared_ptr<Mesh> Mesh::CreateSphereMesh(
        ID3D11Device* device,
        uint32_t sliceCount, uint32_t stackCount)
    {
        eastl::shared_ptr<Mesh> mesh = eastl::make_shared<Mesh>();
        eastl::vector<Vertex> vertices;
        eastl::vector<uint32_t> indices;

        FillSphereMesh(vertices, indices, sliceCount, stackCount);
        mesh->m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
        mesh->m_meshPath = AssetPath(eastl::wstring(L"Sphere"));
        mesh->m_indexCount = static_cast<UINT>(indices.size());
        mesh->m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
        mesh->m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), mesh->m_indexCount);
        return mesh;
    }

    eastl::shared_ptr<Mesh> Mesh::CreateGeosphereMesh(
        ID3D11Device* device,
        uint32_t numSubdivisions)
    {
        eastl::shared_ptr<Mesh> mesh = eastl::make_shared<Mesh>();
        eastl::vector<Vertex> vertices;
        eastl::vector<uint32_t> indices;

        FillGeosphereMesh(vertices, indices, numSubdivisions);
        mesh->m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        mesh->m_meshPath = AssetPath(eastl::wstring(L"Geosphere"));
        mesh->m_indexCount = static_cast<UINT>(indices.size());
        mesh->m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
        mesh->m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), mesh->m_indexCount);
        return mesh;
    }

    eastl::shared_ptr<Mesh> Mesh::CreateCylinderMesh(ID3D11Device* device, uint32_t sliceCount)
    {
        eastl::shared_ptr<Mesh> mesh = eastl::make_shared<Mesh>();
        eastl::vector<Vertex> vertices;
        eastl::vector<uint32_t> indices;

        FillCylinderMesh(vertices, indices, sliceCount);
        mesh->m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        mesh->m_meshPath = AssetPath(eastl::wstring(L"Cylinder"));
        mesh->m_indexCount = static_cast<UINT>(indices.size());
        mesh->m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
        mesh->m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), mesh->m_indexCount);
        return mesh;
    }

    eastl::shared_ptr<Mesh> Mesh::CreateScreenAlignedQuad(ID3D11Device* device)
    {
        eastl::shared_ptr<Mesh> mesh = eastl::make_shared<Mesh>();
        eastl::vector<Vertex> vertices;
        eastl::vector<uint32_t> indices;

        FillScreenAlignedQuad(vertices, indices);
        mesh->m_topology = eastl::make_unique<Bind::Topology>(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        mesh->m_meshPath = AssetPath(eastl::wstring(L"ScreenAlignedQuad"));
        mesh->m_indexCount = static_cast<UINT>(indices.size());
        mesh->m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
        mesh->m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), mesh->m_indexCount);
        return mesh;
    }

    void Mesh::Bind(ID3D11DeviceContext* context) const
    {
        //UINT stride = sizeof(Vertex);
        //UINT offset = 0;
        //context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
        //context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        if (m_vertexBuffer)
            m_vertexBuffer->Bind(context);
        if (m_indexBuffer)
            m_indexBuffer->Bind(context);
        if (m_topology)
            m_topology->Bind(context);
    }

    void Mesh::Draw(ID3D11DeviceContext* context) const
    {
        context->DrawIndexed(m_indexCount, 0, 0);
    }

    void Mesh::ClearMesh()
    {
        if (m_vertexBuffer)
            m_vertexBuffer->Release();
        if (m_indexBuffer)
            m_indexBuffer->Release();
        m_indexCount = 0;
        m_meshPath.m_assetRelativePath.clear();
    }

    void Mesh::Release()
    {
        ClearMesh();
    }

    AssetPath Mesh::GetCurrentMeshPath()
    {
        return m_meshPath;
    }
    SunshineResource::ResourceType Mesh::GetType() const
    {
        return SunshineResource::ResourceType::MESH;
    }
    ResourceGUID Mesh::GetGUID() const
    {
        return m_GUID;
    }
    size_t Mesh::GetSizeInMemory() const
    {
        return m_MemorySize;
    }
}