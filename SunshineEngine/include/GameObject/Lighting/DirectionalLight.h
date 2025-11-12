#pragma once


#include <d3d11.h>
#include <SimpleMath.h>
#include <EASTL/shared_ptr.h>
#include <Graphics/GraphicsResources/VertexShader.h>

#include <GameObject/GameObject.h>

#include <GameObject/Lighting/LightObject.h>
#include <Graphics/Lighting/LightData.h>

namespace DXSM = DirectX::SimpleMath;

class DirectionalLight :
    public LightObject<SE_G::DirectionalLightData>
{
public:

    DirectionalLight(
        SE_G::DirectionalLightData directionalLightData =
        {
            DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
            DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
            DXSM::Vector3::Zero, 0,
            DXSM::Vector3(0, -1, 0), 0
        });
};

class DirectionalLight_Info :
    public LightObject_Info<SE_G::DirectionalLightData>
{
public:

    DirectionalLight_Info(
        SE_G::DirectionalLightData directionalLightData =
        {
            DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
            DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
            DXSM::Vector3::Zero, 0,
            DXSM::Vector3(0, -1, 0), 0
        });

    // Serialization
    json ToJson() const override;
    static eastl::unique_ptr<DirectionalLight_Info> FromJson(const json& j);
};
