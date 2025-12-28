#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <GameObject/GameObject.h>
#include <GameObject/Lighting/LightObject.h>
#include <Graphics/Lighting/LightData.h>

#include <Serialization/LightDataSerialization.h>
#include <Serialization/DXSMSerialization.h>

#include <Utils/AssetPath.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace SE_G {
    class DeferredRenderer;
    class Camera;
};

namespace DXSM = DirectX::SimpleMath;

class SkyBox :
    public LightObject<SE_G::SkyBoxData>
{
public:
    SkyBox(SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        AssetPath assetPath = AssetPath(L"DefaultSkybox.dds", AssetPath::AssetSource::Engine),
        SE_G::SkyBoxData initData = { DXSM::Vector3::One , 0.0f });

    SkyBox(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        const json& j);
};


class SkyBox_Info :
    public LightObject_Info<SE_G::SkyBoxData>
{
public:
    SkyBox_Info(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        AssetPath assetPath = AssetPath(L"DefaultSkybox.dds", AssetPath::AssetSource::Engine),
        SE_G::SkyBoxData initData = { DXSM::Vector3::One , 0.0f });

    SkyBox_Info(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        const json& j);
};