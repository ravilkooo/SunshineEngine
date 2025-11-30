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

class PointLight :
    public LightObject<SE_G::PointLightData>
{
public:

    PointLight(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        SE_G::PointLightData initData = {
            DXSM::Vector3::One, 1.0f,
            DXSM::Vector3::One, 1.0f,
            DXSM::Vector3::Zero, 20,
            DXSM::Vector3::One, 0
        });

    PointLight(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        const json& j);
    // Unnecessary?
    //void UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
};
    
class PointLight_Info :
    public LightObject_Info<SE_G::PointLightData>
{
public:

    PointLight_Info(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        SE_G::PointLightData initData = {
            DXSM::Vector3::One, 1.0f,
            DXSM::Vector3::One, 1.0f,
            DXSM::Vector3::Zero, 20,
            DXSM::Vector3::One, 0
        });

    PointLight_Info(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        const json& j);

    // Unnecessary?
    //void UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
};
