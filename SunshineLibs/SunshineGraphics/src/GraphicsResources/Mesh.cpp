#include "GraphicsResources/Mesh.h"

#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;


Mesh::Mesh(ID3D11Device* device, const std::string& path)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    UINT attrFlags = VertexAttributesFlags::POSITION | VertexAttributesFlags::UV | VertexAttributesFlags::NORMAL;

    if (!LoadModel(vertices, indices, path, attrFlags)) {
        CreateUnwrappedCubeMesh_repeat(vertices, indices);
        //CreateUnwrappedCubeMesh(vertices, indices);
    }
        
    m_indexCount = static_cast<UINT>(indices.size());

    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = UINT(sizeof(Vertex) * vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vinitData = {};
    vinitData.pSysMem = vertices.data();

    HRESULT hr = device->CreateBuffer(&vbd, &vinitData, m_vertexBuffer.GetAddressOf());
    if (FAILED(hr))
        throw std::runtime_error("Failed to create vertex buffer");

    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = indices.data();

    hr = device->CreateBuffer(&ibd, &iinitData, m_indexBuffer.GetAddressOf());
    if (FAILED(hr))
        throw std::runtime_error("Failed to create index buffer");

    D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
    rastDesc.CullMode = D3D11_CULL_BACK;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    device->CreateRasterizerState(&rastDesc, &m_rasterizer);


    D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
    dsDesc.DepthEnable = TRUE;
    // dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    device->CreateDepthStencilState(&dsDesc, &m_depthState);

}

Mesh::~Mesh()
{
    Release();
}

void Mesh::CreateUnwrappedCubeMesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
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
        Vector3 normal;
        Vector3 v0, v1, v2, v3; // 4 вершины (по часовой стрелке)
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
        XMFLOAT3 pos[4] = { faces[f].v0, faces[f].v1, faces[f].v2, faces[f].v3 };
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

void Mesh::CreateUnwrappedCubeMesh_repeat(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    // Развёртка "крестом" (UV-карта 3x4 квадрата, каждая грань в своём прямоугольнике)
    // Текстурные координаты (U, V) для каждой из 6 граней
    // Порядок граней: +X, -X, +Y, -Y, +Z, -Z


    // Вспомогательные данные для каждой грани
    struct Face {
        Vector3 normal;
        Vector3 v0, v1, v2, v3; // 4 вершины (по часовой стрелке)
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
        XMFLOAT3 pos[4] = { faces[f].v0, faces[f].v1, faces[f].v2, faces[f].v3 };
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

bool Mesh::LoadModel(std::vector<Vertex>& vertices,
    std::vector<uint32_t>& indices,
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
                XMFLOAT3(
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
                    XMFLOAT3(
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

void Mesh::Draw(ID3D11DeviceContext* context) const
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->RSSetState(m_rasterizer.Get());
    context->OMSetDepthStencilState(m_depthState.Get(), 0);

    context->DrawIndexed(m_indexCount, 0, 0);
}

void Mesh::Release()
{
    m_vertexBuffer.ReleaseAndGetAddressOf();
    m_indexBuffer.ReleaseAndGetAddressOf();
    m_rasterizer.ReleaseAndGetAddressOf();
    m_depthState.ReleaseAndGetAddressOf();
    m_indexCount = 0;
}
