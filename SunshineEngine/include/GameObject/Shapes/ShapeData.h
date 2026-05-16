#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;

struct PlaneShapeData
{ ; };

struct BoxShapeData {
	bool UvCubeMapMode = false;
};

struct SphereShapeData {
	uint32_t SliceCount = 10u;
	uint32_t StackCount = 10u;
};

struct GeosphereShapeData {
	uint32_t NumSubdivisions = 6u;
};

struct CylinderShapeData {
	uint32_t SliceCount = 10u;
};
