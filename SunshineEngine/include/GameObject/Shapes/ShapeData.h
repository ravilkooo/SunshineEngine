#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;

struct BoxShapeData {
	DXSM::Vector3 Size = DXSM::Vector3::One;
};

struct SphereShapeData {
	DXSM::Vector3 Size = DXSM::Vector3::One;
	uint32_t SliceCount = 10u;
	uint32_t StackCount = 10u;
};

struct GeosphereShapeData {
	DXSM::Vector3 Size = DXSM::Vector3::One;
	uint32_t NumSubdivisions = 6u;
};
