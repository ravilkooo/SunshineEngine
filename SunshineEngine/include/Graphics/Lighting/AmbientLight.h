#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <EASTL/shared_ptr.h>

#include "GameObject.h"

#include "LightObject.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

class AmbientLight :
    public LightObject<SE_G::AmbientLightData>
{
public:
    AmbientLight(SE_G::AmbientLightData initData = { DXSM::Vector3::One, 1.0f });
};

class AmbientLight_Info :
    public LightObject_Info<SE_G::AmbientLightData>
{
public:
    AmbientLight_Info(SE_G::AmbientLightData initData = { DXSM::Vector3::One, 1.0f });

    // Serialization
    json ToJson() const override;
    static eastl::unique_ptr<AmbientLight_Info> FromJson(const json& j);
};