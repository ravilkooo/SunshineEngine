#include "GameObject/Lighting/SpotLight.h"

#include <Graphics/Renderer/Technique/SpotLightTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>
#include <Graphics/Renderer/DeferredRenderer.h>

#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>

#include <Serialization/LightDataSerialization.h>

SpotLight::SpotLight(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    SE_G::SpotLightData initData)
{
    if (initData.Att.z < 0.0001) {
        float c = eastl::max(eastl::max(initData.Diffuse.x, initData.Diffuse.y), initData.Diffuse.z) / initData.Att.y;
        initData.Range = eastl::max(initData.Range, (256.0f * c)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }
    else {
        float c = eastl::max(eastl::max(initData.Diffuse.x, initData.Diffuse.y), initData.Diffuse.z) / initData.Att.z;
        initData.Range = eastl::max(initData.Range, (16.0f * sqrtf(c) + 1.0f)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }

    m_lightData = eastl::make_shared<SE_G::SpotLightData>(initData);

    auto device = renderSystem->GetDevice();

    // TransformComponent
    auto tc = eastl::make_shared<TransformComponent>(device);
    tc->m_position = initData.Position;

    // RenderComponent and Passes
    auto rc = eastl::make_shared<RenderComponent>(m_UUID, renderSystem);

    // LightPass - LightTechnique
    auto lightTech =
        eastl::make_unique<SE_G::SpotLightTechnique>(device, tc.get(), "LightPass", camera, m_lightData);
    rc->AddTechnique(eastl::move(lightTech));

}

SpotLight::SpotLight(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{
    m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
    m_lightData = eastl::make_shared<SE_G::SpotLightData>(j["m_lightData"]);
    m_name = "SpotLight";

    auto device = renderSystem->GetDevice();

    // TransformComponent
    auto tc = AddComponent<TransformComponent>(device);
    if (j["components"].contains("Transform")) {
        tc->FromJson(j["components"]["Transform"]);
    }
    tc->m_position = m_lightData->Position;

    // RenderComponent and Passes
    auto rc = AddComponent<RenderComponent>(m_UUID, renderSystem);

    // LightPass - LightTechnique
    auto lightTech =
        eastl::make_unique<SE_G::SpotLightTechnique>(device, tc.get(), "LightPass", camera, m_lightData);
    rc->AddTechnique(eastl::move(lightTech));
}

SpotLight_Info::SpotLight_Info(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    SE_G::SpotLightData initData)
{
    if (initData.Att.z < 0.0001) {
        float c = eastl::max(eastl::max(initData.Diffuse.x, initData.Diffuse.y), initData.Diffuse.z) / initData.Att.y;
        initData.Range = eastl::max(initData.Range, (256.0f * c)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }
    else {
        float c = eastl::max(eastl::max(initData.Diffuse.x, initData.Diffuse.y), initData.Diffuse.z) / initData.Att.z;
        initData.Range = eastl::max(initData.Range, (16.0f * sqrtf(c) + 1.0f)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }

    m_lightData = eastl::make_shared<SE_G::SpotLightData>(initData);
    m_name = "SpotLight";
    m_group = GameObjectGroup::Lighting;
    m_type.m_asLight = LightObjectType::SpotLight;

    auto device = renderSystem->GetDevice();

    // TransformComponent
    auto tc_info = AddComponent<TransformComponent_Info>(device);
    tc_info->m_assignedComponent->m_position = initData.Position;

    // RenderComponent and Passes
    auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

    // LightPass - LightTechnique
    auto lightTech =
        eastl::make_unique<SE_G::SpotLightTechnique>(device, tc_info->m_assignedComponent.get(), "LightPass", camera, m_lightData);
    rc_info->AddTechnique(eastl::move(lightTech));

    // IconPass
    auto iconTech =
        eastl::make_unique<SE_G::IconTechnique>(device, tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
            SE_G::IconData{ 6u, 0u, 1u, 1u, m_UUID.GetHilo() });

    rc_info->AddTechnique(eastl::move(iconTech));
}

SpotLight_Info::SpotLight_Info(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{
    m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
    m_lightData = eastl::make_shared<SE_G::SpotLightData>(j["m_lightData"]);
    m_name = "SpotLight";
    m_group = GameObjectGroup::Lighting;
    m_type.m_asLight = LightObjectType::SpotLight;

    auto device = renderSystem->GetDevice();

    // TransformComponent
    auto tc_info = AddComponent<TransformComponent_Info>(device);
    if (j["components"].contains("Transform")) {
        tc_info->FromJson(j["components"]["Transform"], device);
    }
    tc_info->m_assignedComponent->m_position = m_lightData->Position;

    // RenderComponent and Passes
    auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

    // LightPass - LightTechnique
    auto lightTech =
        eastl::make_unique<SE_G::SpotLightTechnique>(device, tc_info->m_assignedComponent.get(), "LightPass", camera, m_lightData);
    rc_info->AddTechnique(eastl::move(lightTech));

    // IconPass
    auto iconTech =
        eastl::make_unique<SE_G::IconTechnique>(device, tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
            SE_G::IconData{ 3u, 0u, 1u, 1u, m_UUID.GetHilo() });

    rc_info->AddTechnique(eastl::move(iconTech));
}
