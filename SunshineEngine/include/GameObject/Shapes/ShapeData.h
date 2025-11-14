#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;

struct BoxShapeData {
	DXSM::Vector3 Size;
};

struct SphereShapeData {
	DXSM::Vector3 Size;
	uint32_t SliceCount;
	uint32_t StackCount;
};

struct GeosphereShapeData {
	DXSM::Vector3 Size;
	uint32_t NumSubdivisions;
};
