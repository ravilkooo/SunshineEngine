#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <GameObject/GameObject.h>
#include <GameObject/Lighting/LightObject.h>
#include <Graphics/Lighting/LightData.h>

#include <Utils/AssetPath.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace SE_G {
    class DeferredRenderer;
    class Camera;

    class SkyBoxTechnique;

    namespace Bind {
        class Texture;
    }
};

namespace DXSM = DirectX::SimpleMath;

class SkyBox :
    public LightObject<SE_G::SkyBoxData>
{
public:
    SkyBox(SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        AssetPath assetPath = AssetPath(eastl::wstring(L"Textures/DefaultSkybox.dds"), AssetPath::AssetSource::Engine),
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
        AssetPath assetPath = AssetPath(eastl::wstring(L"Textures/DefaultSkybox.dds"), AssetPath::AssetSource::Engine),
        SE_G::SkyBoxData initData = { DXSM::Vector3::One , 0.0f });

    SkyBox_Info(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        const json& j);

    void SetTexture(eastl::shared_ptr<SE_G::Bind::Texture> tex);
    void SetTexture(SE_G::DeferredRenderer* renderSystem, AssetPath assetPath);

    SE_G::SkyBoxTechnique* m_lightTech = nullptr;
};