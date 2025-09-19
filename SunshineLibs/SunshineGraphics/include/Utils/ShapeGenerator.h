#pragma once

#include <EASTL/algorithm.h>

#include <d3d11.h>
#include <directxmath.h>
#include "GraphicsUtils/SceneNode.h"
#include "GraphicsUtils/CommonVertex.h"

DirectX::XMVECTOR NormalizeHomogeneousVector(DirectX::XMVECTOR vec);

void CreateRandomHeightPlane(float width, float depth, UINT widthSegments, UINT depthSegments, float maxHeight, DirectX::XMFLOAT4 col,
	CommonVertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum);

void CreateSimpleCubeMesh(float width, float height, float depth, DirectX::XMFLOAT4 col,
	CommonVertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum);

void CreateSimpleGeosphereMesh(float radius, DirectX::XMFLOAT4 col,
	CommonVertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum);

void CreateSimpleSphereMesh(float radius, UINT sliceCount, UINT elevationCount,
	DirectX::XMFLOAT4 col,
	CommonVertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum);

void CreateRingMesh(float radius, float thickness, float width,
	UINT sliceCount, DirectX::XMFLOAT4 col,
	CommonVertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum);
