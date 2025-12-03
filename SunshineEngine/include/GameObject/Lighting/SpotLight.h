#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <EASTL/algorithm.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <Graphics/Utils/ShapeGenerator.h>
//#include <DirectXCollision.h>

#include "Graphics/Utils/Camera.h"

#include <GameObject/GameObject.h>
#include <GameObject/Lighting/LightObject.h>
#include <Graphics/Lighting/LightData.h>

#include <Serialization/LightDataSerialization.h>
#include <Serialization/DXSMSerialization.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace DXSM = DirectX::SimpleMath;

class SpotLight :
    public LightObject<SE_G::SpotLightData>
{
public:

    SpotLight(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        SE_G::SpotLightData initData = {
            DXSM::Vector3::One, 1.0f,
            DXSM::Vector3::One, 1.0f,
            DXSM::Vector3::Zero, 20,
            DXSM::Vector2(0, -DX::XM_PIDIV4), 10, 0,
            DXSM::Vector3::One, 0
        });

    SpotLight(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        const json& j);
    // Unnecessary?
    //void UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
};

class SpotLight_Info :
    public LightObject_Info<SE_G::SpotLightData>
{
public:

    SpotLight_Info(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        SE_G::SpotLightData initData = {
            DXSM::Vector3::One, 1.0f,
            DXSM::Vector3::One, 1.0f,
            DXSM::Vector3::Zero, 20,
            DXSM::Vector2(0, -DX::XM_PIDIV4), 10, 0,
            DXSM::Vector3::One, 0
        });

    SpotLight_Info(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        const json& j);

    // Unnecessary?
    //void UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
};
