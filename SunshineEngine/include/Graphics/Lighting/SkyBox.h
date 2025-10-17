#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <EASTL/shared_ptr.h>

#include "GameObject.h"
#include "LightObject.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

class SkyBox :
    public LightObject
{
public:
    eastl::shared_ptr<SkyBoxData> skyBoxData;

    SkyBox(SkyBoxData initData = { DXSM::Vector3::One , 0.0f });
};

