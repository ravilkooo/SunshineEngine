#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <GameObject/GameObject.h>
#include <GameObject/Lighting/LightObject.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include <Graphics/Lighting/LightData.h>

#include <Serialization/LightDataSerialization.h>
#include <Serialization/DXSMSerialization.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace DXSM = DirectX::SimpleMath;

class DirectionalLight :
    public LightObject<SE_G::DirectionalLightData>
{
public:

    DirectionalLight(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        SE_G::DirectionalLightData initData = {
            DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
            DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
            DXSM::Vector3::Zero, 0,
            DXSM::Vector3(1, -1, 1), 0
        }, bool castsShadow = false);

    DirectionalLight(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        const json& j);
};

class DirectionalLight_Info :
    public LightObject_Info<SE_G::DirectionalLightData>
{
public:

    DirectionalLight_Info(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        SE_G::DirectionalLightData initData = {
            DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
            DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
            DXSM::Vector3::Zero, 0,
            DXSM::Vector3(1, -1, 1), 0
        });
    DirectionalLight_Info(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        const json& j);
};
