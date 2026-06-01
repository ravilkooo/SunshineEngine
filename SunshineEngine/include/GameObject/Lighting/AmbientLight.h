#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <GameObject/GameObject.h>
#include <GameObject/Lighting/LightObject.h>
#include <Graphics/Lighting/LightData.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace DXSM = DirectX::SimpleMath;

class AmbientLight :
    public LightObject<SE_G::AmbientLightData>
{
public:
    AmbientLight(
        SE_G::DeferredRenderer* renderSystem,
        SE_G::AmbientLightData initData = { DXSM::Vector3::One * 0.1f, 1.0f });

    AmbientLight(
        SE_G::DeferredRenderer* renderSystem,
        const json& j);
};

class AmbientLight_Info :
    public LightObject_Info<SE_G::AmbientLightData>
{
public:
    AmbientLight_Info(
        SE_G::DeferredRenderer* renderSystem,
        SE_G::AmbientLightData initData = { DXSM::Vector3::One * 0.1f, 1.0f });

    AmbientLight_Info(
        SE_G::DeferredRenderer* renderSystem,
        const json& j);
};