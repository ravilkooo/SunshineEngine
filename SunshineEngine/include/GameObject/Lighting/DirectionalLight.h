#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <GameObject/GameObject.h>
#include <GameObject/Lighting/LightObject.h>
#include <Graphics/Lighting/LightData.h>
#include <Graphics/Renderer/Technique/DirectionalLightTechnique.h>

#include <Serialization/LightDataSerialization.h>
#include <Serialization/DXSMSerialization.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace DXSM = DirectX::SimpleMath;

namespace SE_G
{
    class ShadowMapPass;
}

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
        }, bool castsShadow = true);

    DirectionalLight(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        const json& j);

    void EnableShadow(
        SE_G::DeferredRenderer* renderSystem);
    void DisableShadow();

private:
    SE_G::DirectionalLightTechnique* m_lightTech;
    SE_G::ShadowMapPass* m_shadowMapPass;
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
        }, bool castsShadow = true);

    DirectionalLight_Info(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera,
        const json& j);

    void EnableShadow(
        SE_G::DeferredRenderer* renderSystem);
    void DisableShadow();

    json ToJson() const override {
        json j = LightObject_Info<SE_G::DirectionalLightData>::ToJson();
        j["CastsShadow"] = m_lightTech->m_castsShadow;
        return j;
    }

private:
    SE_G::DirectionalLightTechnique* m_lightTech;
    SE_G::ShadowMapPass* m_shadowMapPass;
};
