

#include "Utils/ShapeGenerator.h"
#include <iostream>
#include <random> // Для генерации случайных чисел

DirectX::XMVECTOR NormalizeHomogeneousVector(DirectX::XMVECTOR vec) {
    // Получаем компонент w
    DirectX::XMVECTOR w = DirectX::XMVectorSplatW(vec);

    DirectX::XMVECTOR cartesian = DirectX::XMVectorDivide(vec, w);

    DirectX::XMVECTOR normalized = DirectX::XMVector3Normalize(cartesian);

    return DirectX::XMVectorSetW(normalized, 1.0f);
}


void CreateRandomHeightPlane(float width, float depth, UINT widthSegments, UINT depthSegments, float maxHeight, DirectX::XMFLOAT4 col,
    CommonVertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum) {

    // Минимальное количество сегментов
    widthSegments = eastl::max(widthSegments, 1u);
    depthSegments = eastl::max(depthSegments, 1u);

    // Количество вершин
    *verticesNum = (widthSegments + 1) * (depthSegments + 1);
    *vertices = (CommonVertex*)calloc(*verticesNum, sizeof(CommonVertex));

    // Количество индексов (по два треугольника на каждый квадрат)
    *indicesNum = widthSegments * depthSegments * 6;
    *indices = (int*)calloc(*indicesNum, sizeof(int));

    // Шаг между вершинами
    float widthStep = width / widthSegments;
    float depthStep = depth / depthSegments;

    // Генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-maxHeight, maxHeight);

    // Генерация вершин
    for (UINT i = 0; i <= depthSegments; ++i) {
        for (UINT j = 0; j <= widthSegments; ++j) {
            float x = -width / 2.0f + j * widthStep; // Центрируем плоскость по X
            float z = -depth / 2.0f + i * depthStep; // Центрируем плоскость по Z
            float y = dis(gen); // Случайное отклонение по Y

            (*vertices)[i * (widthSegments + 1) + j] = {
                DirectX::XMFLOAT3(x, y, z),
                ((i + j) % 2 ? col : XMFLOAT4(1.0f - col.x, 1.0f - col.y, 1.0f - col.z, 1.0f)),
                XMFLOAT2(i * 1.0f / depthSegments, j * 1.0f / widthSegments),
                XMFLOAT3(0,1,0)
            };
        }
    }

    // Генерация индексов
    UINT index = 0;
    for (UINT i = 0; i < depthSegments; ++i) {
        for (UINT j = 0; j < widthSegments; ++j) {
            UINT topLeft = i * (widthSegments + 1) + j;
            UINT topRight = topLeft + 1;
            UINT bottomLeft = (i + 1) * (widthSegments + 1) + j;
            UINT bottomRight = bottomLeft + 1;

            // Первый треугольник
            (*indices)[index++] = topLeft;
            (*indices)[index++] = bottomLeft;
            (*indices)[index++] = topRight;

            // Второй треугольник
            (*indices)[index++] = topRight;
            (*indices)[index++] = bottomLeft;
            (*indices)[index++] = bottomRight;
        }
    }

    return;
}

void CreateSimpleCubeMesh(float width, float height, float depth, DirectX::XMFLOAT4 col,
    CommonVertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum) {
    float w2 = 0.5f * width;
    float h2 = 0.5f * height;
    float d2 = 0.5f * depth;

    //DirectX::XMFLOAT4 col_1 = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    //DirectX::XMFLOAT4 col_2 = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    *vertices = (CommonVertex*)calloc(24, sizeof(CommonVertex));
    *verticesNum = 24;

    (*vertices)[0] = { DirectX::XMFLOAT3(-w2, -h2, -d2), col, {0, 0, -1} };
    (*vertices)[1] = { DirectX::XMFLOAT3(-w2, +h2, -d2), col, {0, 0, -1} };
    (*vertices)[2] = { DirectX::XMFLOAT3(+w2, +h2, -d2), col, {0, 0, -1} };
    (*vertices)[3] = { DirectX::XMFLOAT3(+w2, -h2, -d2), col, {0, 0, -1} };

    (*vertices)[4] = { DirectX::XMFLOAT3(-w2, -h2, +d2), col, {0, 0, 1} };
    (*vertices)[5] = { DirectX::XMFLOAT3(-w2, +h2, +d2), col, {0, 0, 1} };
    (*vertices)[6] = { DirectX::XMFLOAT3(+w2, +h2, +d2), col, {0, 0, 1} };
    (*vertices)[7] = { DirectX::XMFLOAT3(+w2, -h2, +d2), col, {0, 0, 1} };

    (*vertices)[8] = { DirectX::XMFLOAT3(-w2, -h2, +d2), col, {-1, 0, 0} }; // 4
    (*vertices)[9] = { DirectX::XMFLOAT3(-w2, +h2, +d2), col, {-1, 0, 0} }; // 5
    (*vertices)[10] = { DirectX::XMFLOAT3(-w2, +h2, -d2), col, {-1, 0, 0} }; // 1
    (*vertices)[11] = { DirectX::XMFLOAT3(-w2, -h2, -d2), col, {-1, 0, 0} }; // 0

    (*vertices)[12] = { DirectX::XMFLOAT3(+w2, -h2, -d2), col, {1, 0, 0} }; // 3
    (*vertices)[13] = { DirectX::XMFLOAT3(+w2, +h2, -d2), col, {1, 0, 0} }; // 2
    (*vertices)[14] = { DirectX::XMFLOAT3(+w2, +h2, +d2), col, {1, 0, 0} }; // 6
    (*vertices)[15] = { DirectX::XMFLOAT3(+w2, -h2, +d2), col, {1, 0, 0} }; // 7

    (*vertices)[16] = { DirectX::XMFLOAT3(-w2, +h2, -d2), col, {0, 1, 0} }; // 1
    (*vertices)[17] = { DirectX::XMFLOAT3(-w2, +h2, +d2), col, {0, 1, 0} }; // 5
    (*vertices)[18] = { DirectX::XMFLOAT3(+w2, +h2, +d2), col, {0, 1, 0} }; // 6
    (*vertices)[19] = { DirectX::XMFLOAT3(+w2, +h2, -d2), col, {0, 1, 0} }; // 2

    (*vertices)[20] = { DirectX::XMFLOAT3(-w2, -h2, +d2), col, {0, -1, 0} }; // 4
    (*vertices)[21] = { DirectX::XMFLOAT3(-w2, -h2, -d2), col, {0, -1, 0} }; // 0
    (*vertices)[22] = { DirectX::XMFLOAT3(+w2, -h2, -d2), col, {0, -1, 0} }; // 3
    (*vertices)[23] = { DirectX::XMFLOAT3(+w2, -h2, +d2), col, {0, -1, 0} }; // 7


    int _indices[36] = {
        0,1,2, // ok
        0,2,3, // ok

        4,6,5, // ok
        4,7,6, // ok

        8,9,10, // ok
        8,10,11, // ok

        12,13,14, // ok
        12,14,15, // ok

        16,17,18, // ok
        16,18,19, // ok

        20,21,22, // ok
        20,22,23, // ok
    };

    *indices = (int*)calloc(36, sizeof(int));
    *indicesNum = 36;

    for (size_t i = 0; i < 36; i++)
    {
        (*indices)[i] = _indices[i];
    }

    return;
}

void CreateSimpleGeosphereMesh(float radius, DirectX::XMFLOAT4 col,
    CommonVertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum) {
    

    //numSubdivisions = MathHelper::Min(numSubdivisions, 5u);

    const float X = 0.525731f;
    const float Z = 0.850651f;
    DirectX::XMFLOAT3 pos[12] =
    {
        DirectX::XMFLOAT3(-X, 0.0f, Z), DirectX::XMFLOAT3(X, 0.0f, Z),
        DirectX::XMFLOAT3(-X, 0.0f, -Z), DirectX::XMFLOAT3(X, 0.0f, -Z),
        DirectX::XMFLOAT3(0.0f, Z, X), DirectX::XMFLOAT3(0.0f, Z, -X),
        DirectX::XMFLOAT3(0.0f, -Z, X), DirectX::XMFLOAT3(0.0f, -Z, -X),
        DirectX::XMFLOAT3(Z, X, 0.0f), DirectX::XMFLOAT3(-Z, X, 0.0f),
        DirectX::XMFLOAT3(Z, -X, 0.0f), DirectX::XMFLOAT3(-Z, -X, 0.0f)
    };
    DWORD k[60] =
    {
    1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
    1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
    3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
    10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
    };
    

    *vertices = (CommonVertex*)calloc(12, sizeof(CommonVertex));
    *verticesNum = 12;
    for (size_t i = 0; i < 12; i++) {
        (*vertices)[i] = { pos[i], col };
    }

    *indicesNum = 60;
    *indices = (int*)calloc(60, sizeof(int));
    for (size_t i = 0; i < 60; ++i)
        (*indices)[i] = k[i];

    
    //for (size_t i = 0; i < numSubdivisions; ++i)
    //    Subdivide(meshData);// Project vertices onto sphere and scale.
    for (size_t i = 0; i < 12; i++)
    {
        // Project onto unit sphere.
        DirectX::XMVECTOR _n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&(*vertices)[i].pos));
        // Project onto sphere.
        DirectX::XMVECTOR _p = DirectX::XMVectorScale (_n, radius);
        //DirectX::XMFLOAT4 _p_f4;
        DirectX::XMStoreFloat3(&((*vertices)[i].pos), _p);
        //std::cout << i << " " << (*vertices)[2 * i].x << ", ";
        //(*vertices)[2 * i] = _p_f4;
    }
    
    return;
}


void CreateSimpleSphereMesh(float radius, UINT sliceCount, UINT elevationCount,
    DirectX::XMFLOAT4 col,
    CommonVertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum) {

    sliceCount = eastl::max(sliceCount, 4u);
    elevationCount = eastl::max(elevationCount, 1u);

    *verticesNum = 2 + (2 * elevationCount + 1) * (sliceCount + 1);
    *vertices = (CommonVertex*)malloc(*verticesNum * sizeof(CommonVertex));

    float sliceStep = DirectX::XM_2PI / sliceCount;
    float elevationStep = DirectX::XM_PIDIV2 / (elevationCount + 1);


    UINT _offsetCommonVertexIdx = 0;
    // top vertex
    (*vertices)[_offsetCommonVertexIdx++] = { DirectX::XMFLOAT3(0.0f, radius, 0.0f), col, XMFLOAT2(0, 0),
    XMFLOAT3(0,1,0)};
    // other vertices
    for (UINT i = 1; i <= 2 * elevationCount + 1; ++i)
    {
        for (UINT j = 0; j <= sliceCount; ++j) {
            (*vertices)[_offsetCommonVertexIdx++] =
            { DirectX::XMFLOAT3(
                radius * sinf(elevationStep * i) * cosf(sliceStep * j),
                radius * cosf(elevationStep * i),
                radius * sinf(elevationStep * i) * sinf(sliceStep * j)
            ),
                col,
                XMFLOAT2( j * 1.0f / sliceCount , (i * 1.0f) / (2 * elevationCount+2)),
                XMFLOAT3(
                    sinf(elevationStep * i) * cosf(sliceStep * j),
                    cosf(elevationStep * i),
                    sinf(elevationStep * i) * sinf(sliceStep * j)
                )};
        }
    }
    // bottom vertex
    (*vertices)[_offsetCommonVertexIdx++] = { DirectX::XMFLOAT3(0.0f, -radius, 0.0f), col, XMFLOAT2(1, 1),
    XMFLOAT3(0,-1,0) };


    *indicesNum = 6 * (sliceCount + 1) + 2 * 6 * elevationCount * (sliceCount + 1);
    //std::cout << *indicesNum << " << \n";
    *indices = (int*)malloc(*indicesNum * sizeof(int));

    UINT indexIndex = 0;

    for (UINT j = 0; j < sliceCount; ++j) {
        (*indices)[indexIndex++] = 0;
        (*indices)[indexIndex++] = j + 2;
        (*indices)[indexIndex++] = j + 1;
    }
    
    (*indices)[indexIndex++] = 0;
    (*indices)[indexIndex++] = 1;
    (*indices)[indexIndex++] = sliceCount;


    for (UINT i = 0; i < 2 * elevationCount; ++i) {
        UINT startIndex = 1 + i * (sliceCount + 1);
        UINT nextStartIndex = startIndex + (sliceCount + 1);
        for (UINT j = 0; j < sliceCount; ++j) {
            
            (*indices)[indexIndex++] = startIndex + j;
            (*indices)[indexIndex++] = startIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j;

            (*indices)[indexIndex++] = startIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j;
        }

        (*indices)[indexIndex++] = startIndex + sliceCount;
        (*indices)[indexIndex++] = startIndex;
        (*indices)[indexIndex++] = nextStartIndex + sliceCount;

        (*indices)[indexIndex++] = startIndex;
        (*indices)[indexIndex++] = nextStartIndex;
        (*indices)[indexIndex++] = nextStartIndex + sliceCount;
    }
    
    
    UINT bottomIndex = _offsetCommonVertexIdx - 1;
    UINT startIndex = 1 + 2 * elevationCount * (sliceCount + 1);
    for (UINT j = 0; j < sliceCount; ++j) {
        (*indices)[indexIndex++] = bottomIndex;
        (*indices)[indexIndex++] = startIndex + j;
        (*indices)[indexIndex++] = startIndex + j + 1;
    }

    (*indices)[indexIndex++] = bottomIndex;
    (*indices)[indexIndex++] = startIndex + sliceCount;
    (*indices)[indexIndex++] = startIndex;

    return;
}

void CreateRingMesh(float radius, float thickness, float width,
    UINT sliceCount,
    DirectX::XMFLOAT4 col,
    CommonVertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum)
{
    sliceCount = eastl::max(sliceCount, 4u);
    thickness = eastl::max(0.0001f, thickness);

    *verticesNum = 4 * sliceCount;
    *vertices = (CommonVertex*)malloc(*verticesNum * sizeof(CommonVertex));

    float sliceStep = DirectX::XM_2PI / sliceCount;

    UINT _offsetCommonVertexIdx = 0;
    // top vertices
    
    {
        float half_thickness = 0.5 * thickness;

        for (UINT j = 0; j < sliceCount; ++j) {
            (*vertices)[_offsetCommonVertexIdx++] =
            { DirectX::XMFLOAT3(
                radius * cosf(sliceStep * j),
                half_thickness,
                radius *  sinf(sliceStep * j)
            ), col };
        }

        float outer_radius = radius + width;

        for (UINT j = 0; j < sliceCount; ++j) {
            (*vertices)[_offsetCommonVertexIdx++] =
            { DirectX::XMFLOAT3(
                outer_radius * cosf(sliceStep * j),
                half_thickness,
                outer_radius * sinf(sliceStep * j)
            ), col };
        }

        for (UINT j = 0; j < sliceCount; ++j) {
            (*vertices)[_offsetCommonVertexIdx++] =
            { DirectX::XMFLOAT3(
                outer_radius * cosf(sliceStep * j),
                -half_thickness,
                outer_radius * sinf(sliceStep * j)
            ), col };
        }

        for (UINT j = 0; j < sliceCount; ++j) {
            (*vertices)[_offsetCommonVertexIdx++] =
            { DirectX::XMFLOAT3(
                radius * cosf(sliceStep * j),
                -half_thickness,
                radius * sinf(sliceStep * j)
            ), col };
        }

    }


    *indicesNum = 6 * 4 * sliceCount;
    //std::cout << *indicesNum << " << \n";
    *indices = (int*)malloc(*indicesNum * sizeof(int));

    UINT indexIndex = 0;
    for (UINT i = 0; i < 3; i++)
    {
        UINT startIndex = i * sliceCount;
        UINT nextStartIndex = startIndex + sliceCount;
        for (UINT j = 0; j < sliceCount - 1; ++j) {

            (*indices)[indexIndex++] = startIndex + j;
            (*indices)[indexIndex++] = startIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j;

            (*indices)[indexIndex++] = startIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j;
        }

        (*indices)[indexIndex++] = startIndex + sliceCount - 1;
        (*indices)[indexIndex++] = startIndex;
        (*indices)[indexIndex++] = nextStartIndex + sliceCount - 1;

        (*indices)[indexIndex++] = startIndex;
        (*indices)[indexIndex++] = nextStartIndex;
        (*indices)[indexIndex++] = nextStartIndex + sliceCount - 1;

    }

    {
        UINT startIndex = 3 * sliceCount;
        UINT nextStartIndex = 0;
        for (UINT j = 0; j < sliceCount - 1; ++j) {

            (*indices)[indexIndex++] = startIndex + j;
            (*indices)[indexIndex++] = startIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j;

            (*indices)[indexIndex++] = startIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j;
        }

        (*indices)[indexIndex++] = startIndex + sliceCount - 1;
        (*indices)[indexIndex++] = startIndex;
        (*indices)[indexIndex++] = nextStartIndex + sliceCount - 1;

        (*indices)[indexIndex++] = startIndex;
        (*indices)[indexIndex++] = nextStartIndex;
        (*indices)[indexIndex++] = nextStartIndex + sliceCount - 1;

    }

    return;
}
