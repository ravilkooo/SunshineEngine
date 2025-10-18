#include "GraphicsResources/Mesh.h"

#include <SimpleMath.h>

Mesh::Mesh(ID3D11Device* device, const std::string& path)
{
    eastl::vector<Vertex> vertices;
    eastl::vector<uint32_t> indices;

    UINT attrFlags = VertexAttributesFlags::POSITION | VertexAttributesFlags::UV | VertexAttributesFlags::NORMAL;

    if (path == "CubeMesh") {
        FillUnwrappedBoxMesh(vertices, indices);
        m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
    else if (path == "Sphere") {
        FillSphereMesh(vertices, indices);
        m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
    else if (path == "Geosphere") {
        FillGeosphereMesh(vertices, indices, 1.0f, 0u);
        m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
    else if (path == "CubeMesh_repeat") {
        FillUnwrappedBoxMesh_repeat(vertices, indices);
        m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
    else if (path == "ScreenAlignedQuad") {
        FillScreenAlignedQuad(vertices, indices);
        m_topology = eastl::make_unique<Bind::Topology>(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    }
    else if (!LoadModel(vertices, indices, path, attrFlags)) {
        FillUnwrappedBoxMesh_repeat(vertices, indices);
        m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    m_indexCount = static_cast<UINT>(indices.size());
    m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
    m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), m_indexCount);

}

Mesh::~Mesh()
{
    Release();
}

void Mesh::FillUnwrappedBoxMesh(
    eastl::vector<Vertex>& vertices,
    eastl::vector<uint32_t>& indices,
    float width, float height, float length)
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
        {1 * du, 1 * dv}, // +Z
        {3 * du, 1 * dv}  // -Z
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
        DX::XMFLOAT3 pos[4] = { faces[f].v0, faces[f].v1, faces[f].v2, faces[f].v3 };
        for (int i = 0; i < 4; ++i)
        {
            Vertex vert;
            vert.position.x = pos[i].x * width;
            vert.position.y = pos[i].y * height;
            vert.position.z = pos[i].z * length;
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

void Mesh::FillUnwrappedBoxMesh_repeat(eastl::vector<Vertex>& vertices,
    eastl::vector<uint32_t>& indices,
    float width, float height, float length)
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
            vert.position.x = pos[i].x * width;
            vert.position.y = pos[i].y * height;
            vert.position.z = pos[i].z * length;
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
    float radius, uint32_t sliceCount, uint32_t stackCount)
{
    vertices.resize(0);
    indices.resize(0);

    Vertex topVertex(
        DXSM::Vector3(0.0f, +radius, 0.0f),
        DXSM::Vector4::One,
        DXSM::Vector2(0.0f, 0.0f),
        DXSM::Vector3(0.0f, +1.0f, 0.0f));

    Vertex bottomVertex(
        DXSM::Vector3(0.0f, -radius, 0.0f),
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

            v.position.x = radius * sinf(phi) * cosf(theta);
            v.position.y = radius * cosf(phi);
            v.position.z = radius * sinf(phi) * sinf(theta);

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
    float radius,
    UINT numSubdivisions)
{
    numSubdivisions = std::min<UINT>(numSubdivisions, 0u);

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

        vertices[i].position = radius * vertices[i].normal;

        // [-pi, pi]
        float theta = std::atan2f(vertices[i].position.z, vertices[i].position.x);

        // Put in [0, 2pi].
        if (theta < 0.0f)
            theta += DX::XM_PI;

        float phi = acosf(vertices[i].position.y / radius);

        // [0,1]
        vertices[i].texcoord.x = theta / DX::XM_2PI;
        vertices[i].texcoord.y = phi / DX::XM_PI;
    }

    return;
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

    uint32_t numTris = (uint32_t) indices_copy.size() / 3;
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
    const std::string& path, UINT attrFlags)
{
    Assimp::Importer importer;
    const aiScene* pModel = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_FlipUVs
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
    ID3D11Device* device,
    float width, float height, float length)
{
    eastl::shared_ptr<Mesh> mesh = eastl::make_shared<Mesh>();
    eastl::vector<Vertex> vertices;
    eastl::vector<uint32_t> indices;

    FillUnwrappedBoxMesh(vertices, indices, width, height, length);
    mesh->m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    mesh->m_indexCount = static_cast<UINT>(indices.size());
    mesh->m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
    mesh->m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), mesh->m_indexCount);
    return mesh;
}

eastl::shared_ptr<Mesh> Mesh::CreateUnwrappedBoxMesh_repeat(
    ID3D11Device* device,
    float width, float height, float length)
{
    eastl::shared_ptr<Mesh> mesh = eastl::make_shared<Mesh>();
    eastl::vector<Vertex> vertices;
    eastl::vector<uint32_t> indices;

    FillUnwrappedBoxMesh_repeat(vertices, indices, width, height, length);
    mesh->m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    mesh->m_indexCount = static_cast<UINT>(indices.size());
    mesh->m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
    mesh->m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), mesh->m_indexCount);
    return mesh;
}

eastl::shared_ptr<Mesh> Mesh::CreateSphereMesh(ID3D11Device* device, float radius, uint32_t sliceCount, uint32_t stackCount)
{
    eastl::shared_ptr<Mesh> mesh = eastl::make_shared<Mesh>();
    eastl::vector<Vertex> vertices;
    eastl::vector<uint32_t> indices;

    FillSphereMesh(vertices, indices, radius, sliceCount, stackCount);
    mesh->m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    mesh->m_indexCount = static_cast<UINT>(indices.size());
    mesh->m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
    mesh->m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), mesh->m_indexCount);
    return mesh;
}

eastl::shared_ptr<Mesh> Mesh::CreateGeosphereMesh(ID3D11Device* device, float radius, UINT numSubdivisions)
{
    eastl::shared_ptr<Mesh> mesh = eastl::make_shared<Mesh>();
    eastl::vector<Vertex> vertices;
    eastl::vector<uint32_t> indices;

    FillGeosphereMesh(vertices, indices, radius, numSubdivisions);
    mesh->m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

    mesh->m_indexCount = static_cast<UINT>(indices.size());
    mesh->m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(device, vertices.data(), vertices.size(), sizeof(Vertex));
    mesh->m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(device, indices.data(), mesh->m_indexCount);
    return mesh;
}

void Mesh::Draw(ID3D11DeviceContext* context) const
{
    //UINT stride = sizeof(Vertex);
    //UINT offset = 0;
    //context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    //context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    m_vertexBuffer->Bind(context);
    m_indexBuffer->Bind(context);
    m_topology->Bind(context);

    context->DrawIndexed(m_indexCount, 0, 0);
}

void Mesh::Release()
{
    //m_vertexBuffer.ReleaseAndGetAddressOf();
    //m_indexBuffer.ReleaseAndGetAddressOf();
    m_indexCount = 0;
}
