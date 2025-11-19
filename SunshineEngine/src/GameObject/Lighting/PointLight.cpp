#include "GameObject/Lighting/PointLight.h"

#include <Graphics/Renderer/Technique/PointLightTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>
#include <Graphics/Renderer/DeferredRenderer.h>

#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>

PointLight::PointLight(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    SE_G::PointLightData initData)
{
    if (initData.Att.z < 0.0001) {
        float c = eastl::max(eastl::max(initData.Diffuse.x, initData.Diffuse.y), initData.Diffuse.z) / initData.Att.y;
        initData.Range = eastl::max(initData.Range, (256.0f * c)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }
    else {
        float c = eastl::max(eastl::max(initData.Diffuse.x, initData.Diffuse.y), initData.Diffuse.z) / initData.Att.z;
        initData.Range = eastl::max(initData.Range, (16.0f * sqrtf(c) + 1.0f)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }

    m_lightData = eastl::make_shared<SE_G::PointLightData>(initData);

    auto device = renderSystem->GetDevice();

    // TransformComponent
    auto tc = eastl::make_shared<TransformComponent>(device);
    tc->m_position = initData.Position;

    // RenderComponent and Passes
    auto rc = eastl::make_shared<RenderComponent>(renderSystem);

    // LightPass - LightTechnique
    auto lightTech =
        eastl::make_unique<SE_G::PointLightTechnique>(device, tc.get(), "LightPass", camera, m_lightData);
    rc->AddTechnique(eastl::move(lightTech));

}

PointLight::PointLight(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{
    m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
    m_lightData = eastl::make_shared<SE_G::PointLightData>(j["m_lightData"]);
    m_name = "PointLight";

    auto device = renderSystem->GetDevice();

    // TransformComponent
    auto tc = AddComponent<TransformComponent>(device);
    if (j["components"].contains("Transform")) {
        tc->FromJson(j["components"]["Transform"]);
    }
    tc->m_position = m_lightData->Position;

    // RenderComponent and Passes
    auto rc = AddComponent<RenderComponent>(renderSystem);

    // LightPass - LightTechnique
    auto lightTech =
        eastl::make_unique<SE_G::PointLightTechnique>(device, tc.get(), "LightPass", camera, m_lightData);
    rc->AddTechnique(eastl::move(lightTech));
}

PointLight_Info::PointLight_Info(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    SE_G::PointLightData initData)
{
    if (initData.Att.z < 0.0001) {
        float c = eastl::max(eastl::max(initData.Diffuse.x, initData.Diffuse.y), initData.Diffuse.z) / initData.Att.y;
        initData.Range = eastl::max(initData.Range, (256.0f * c)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }
    else {
        float c = eastl::max(eastl::max(initData.Diffuse.x, initData.Diffuse.y), initData.Diffuse.z) / initData.Att.z;
        initData.Range = eastl::max(initData.Range, (16.0f * sqrtf(c) + 1.0f)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }

    m_lightData = eastl::make_shared<SE_G::PointLightData>(initData);
    m_name = "PointLight";
    m_group = GameObjectGroup::Lighting;
    m_type.m_asLight = LightObjectType::PointLight;

    auto device = renderSystem->GetDevice();

    // TransformComponent
    auto tc_info = AddComponent<TransformComponent_Info>();
    tc_info->m_assignedComponent = eastl::make_unique<TransformComponent>(device);
    tc_info->m_assignedComponent->m_position = initData.Position;

    // RenderComponent and Passes
    auto rc_info = AddComponent<RenderComponent_Info>();
    rc_info->m_assignedComponent = eastl::make_unique<RenderComponent>(renderSystem);

    // LightPass - LightTechnique
    auto lightTech =
        eastl::make_unique<SE_G::PointLightTechnique>(device, tc_info->m_assignedComponent.get(), "LightPass", camera, m_lightData);
    rc_info->AddTechnique(eastl::move(lightTech));

    // IconPass
    auto iconTech =
        eastl::make_unique<SE_G::IconTechnique>(device, tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
            SE_G::IconData{ 3u, 0u, 1u, 1u, m_UUID.GetHilo() });

    rc_info->AddTechnique(eastl::move(iconTech));
}

PointLight_Info::PointLight_Info(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{
    m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
    m_lightData = eastl::make_shared<SE_G::PointLightData>(j["m_lightData"]);
    m_name = "PointLight";
    m_group = GameObjectGroup::Lighting;
    m_type.m_asLight = LightObjectType::PointLight;

    auto device = renderSystem->GetDevice();

    // TransformComponent
    auto tc_info = AddComponent<TransformComponent_Info>();
    if (j["components"].contains("Transform")) {
        tc_info->FromJson(j["components"]["Transform"], device);
    }
    else {
        tc_info->m_assignedComponent = eastl::make_unique<TransformComponent>(device);
    }
    tc_info->m_assignedComponent->m_position = m_lightData->Position;

    // RenderComponent and Passes
    auto rc_info = AddComponent<RenderComponent_Info>();
    rc_info->m_assignedComponent = eastl::make_unique<RenderComponent>(renderSystem);

    // LightPass - LightTechnique
    auto lightTech =
        eastl::make_unique<SE_G::PointLightTechnique>(device, tc_info->m_assignedComponent.get(), "LightPass", camera, m_lightData);
    rc_info->AddTechnique(eastl::move(lightTech));

    // IconPass
    auto iconTech =
        eastl::make_unique<SE_G::IconTechnique>(device, tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
            SE_G::IconData{ 3u, 0u, 1u, 1u, m_UUID.GetHilo() });

    rc_info->AddTechnique(eastl::move(iconTech));
}

//void PointLight::UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
//{
//    //pointLightPBuffer->Update(context.Get(), m_lightData);
//}

/*
D3D11_DEPTH_STENCIL_DESC PointLight::ChooseDepthStencilState(LightObject::LightPosition lightPos)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
    if (lightPos == LightPosition::INSIDE) {
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    }
    else if (lightPos == LightPosition::FILL || lightPos == LightPosition::BEHIND_NEAR_PLANE) {
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    }
    else if (lightPos == LightPosition::INTERSECT_FAR_PLANE) {
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    }
    else { // I dont know why, just
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    }
    return dsDesc;
}

D3D11_RASTERIZER_DESC PointLight::GetRasterizerDesc(LightObject::LightPosition lightPos)
{
    D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
    if (lightPos == LightPosition::INSIDE) {
        rasterDesc.CullMode = D3D11_CULL_FRONT;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
    }
    else if (lightPos == LightPosition::FILL || lightPos == LightPosition::BEHIND_NEAR_PLANE) {
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
    }
    else if (lightPos == LightPosition::INTERSECT_FAR_PLANE) {
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
    }
    else { // I dont know why, just
        rasterDesc.CullMode = D3D11_CULL_FRONT;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
    }
    return rasterDesc;
}
*/