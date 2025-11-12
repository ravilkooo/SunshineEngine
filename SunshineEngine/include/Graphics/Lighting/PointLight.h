#pragma once



#include <d3d11.h>
#include <SimpleMath.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include <GameObject.h>

#include <EASTL/algorithm.h>

#include "LightObject.h"

#include <Graphics/Utils/ShapeGenerator.h>
//#include <DirectXCollision.h>

#include <d3d11.h>
#include <directxmath.h>

#include <EASTL/shared_ptr.h>

#include "Graphics/Utils/Camera.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

class PointLight :
    public LightObject<SE_G::PointLightData>
{
public:

    PointLight(
        SE_G::PointLightData pointLightData =
        {
            DXSM::Vector3::One, 1,
            DXSM::Vector3::One, 1,
            DXSM::Vector3::Zero, 20,
            DXSM::Vector3::One, 0
        });

    // Unnecessary?
    //void UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
};
    
class PointLight_Info :
    public LightObject_Info<SE_G::PointLightData>
{
public:

    PointLight_Info(
        SE_G::PointLightData pointLightData =
        {
            DXSM::Vector3::One, 1,
            DXSM::Vector3::One, 1,
            DXSM::Vector3::Zero, 20,
            DXSM::Vector3::One, 0
        });

    // Unnecessary?
    //void UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

    // Serialization
    json ToJson() const override;
    static eastl::unique_ptr<PointLight_Info> FromJson(const json& j);
};
