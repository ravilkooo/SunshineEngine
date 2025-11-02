#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <EASTL/shared_ptr.h>

#include "GameObject.h"

#include "LightObject.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

class AmbientLight :
    public LightObject<AmbientLightData>
{
public:
    AmbientLight(AmbientLightData initData = { DXSM::Vector3::One, 1.0f });
};


