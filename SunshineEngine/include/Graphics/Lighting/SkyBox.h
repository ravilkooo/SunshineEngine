#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <EASTL/shared_ptr.h>

#include <GameObject.h>
#include "LightObject.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

class SkyBox :
    public LightObject<SE_G::SkyBoxData>
{
public:
    SkyBox(SE_G::SkyBoxData initData = { DXSM::Vector3::One , 0.0f });
};


class SkyBox_Info :
    public LightObject_Info<SE_G::SkyBoxData>
{
public:
    SkyBox_Info(SE_G::SkyBoxData initData = { DXSM::Vector3::One , 0.0f });

    // Serialization
    json ToJson() const override;
    static eastl::unique_ptr<SkyBox_Info> FromJson(const json& j);
};