#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <SimpleMath.h>


namespace DXSM = DirectX::SimpleMath;

class Transformable
{
public:
	virtual DXSM::Matrix GetWorldMatrix() const = 0;
};

