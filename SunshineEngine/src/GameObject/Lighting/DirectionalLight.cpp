#include "GameObject/Lighting/DirectionalLight.h"

#include <Graphics/Renderer/Technique/DirectionalLightTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>
#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/Pass/RenderPass.h>
#include <Graphics/Renderer/Pass/ShadowMapPass.h>

#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>



DirectionalLight::DirectionalLight(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera, SE_G::DirectionalLightData initData,
	bool castsShadow)
{
    initData.Direction.Normalize();

    m_lightData = eastl::make_shared<SE_G::DirectionalLightData>(initData);

    auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = eastl::make_shared<TransformComponent>(device);
	tc->m_position = initData.Position;

	// RenderComponent and Passess
	auto rc = eastl::make_shared<RenderComponent>(m_UUID, renderSystem);

	// LightPass - LightTechnique
	auto lightTech =
		eastl::make_unique<SE_G::DirectionalLightTechnique>(
			device, tc.get(), "LightPass", camera, m_lightData);

	if (castsShadow)
	{
		//renderSystem->
		auto gPass = static_cast<SE_G::GPass*>(renderSystem->GetPass(SE_G::RenderPass::PassType::GPass));

		m_shadowMapPass = static_cast<SE_G::ShadowMapPass*>(
			renderSystem->AddPass(eastl::make_unique<SE_G::ShadowMapPass>(
				renderSystem->GetDevice(), renderSystem->GetDeviceContext(),
				gPass, m_lightData)));

		lightTech->AssignShadowMapPass(m_shadowMapPass);
		lightTech->EnableShadow();
	}

	m_lightTech = static_cast<SE_G::DirectionalLightTechnique*>(rc->AddTechnique(eastl::move(lightTech)));
	m_lightTech->m_castsShadow = castsShadow;

}

DirectionalLight::DirectionalLight(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	const json& j)
{
	m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	m_lightData = eastl::make_shared<SE_G::DirectionalLightData>(j["m_lightData"]);
	m_name = "DirectionalLight";

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
		eastl::make_unique<SE_G::DirectionalLightTechnique>(device, tc.get(), "LightPass", camera, m_lightData);

	bool castsShadow = j.contains("CastsShadow") && j["CastsShadow"];
	if (castsShadow)
	{
		
		auto gPass = static_cast<SE_G::GPass*>(renderSystem->GetPass(SE_G::RenderPass::PassType::GPass));

		m_shadowMapPass = static_cast<SE_G::ShadowMapPass*>(
			renderSystem->AddPass(eastl::make_unique<SE_G::ShadowMapPass>(
				renderSystem->GetDevice(), renderSystem->GetDeviceContext(),
				gPass, m_lightData)));
		lightTech->AssignShadowMapPass(m_shadowMapPass);
		lightTech->EnableShadow();
	}
	m_lightTech = static_cast<SE_G::DirectionalLightTechnique*>(rc->AddTechnique(eastl::move(lightTech)));
	m_lightTech->m_castsShadow = castsShadow;
}

void DirectionalLight::EnableShadow(
	SE_G::DeferredRenderer* renderSystem) {
	if (m_shadowMapPass)
		m_lightTech->EnableShadow();
	else
	{
		auto gPass = static_cast<SE_G::GPass*>(renderSystem->GetPass(SE_G::RenderPass::PassType::GPass));

		m_shadowMapPass = static_cast<SE_G::ShadowMapPass*>(
			renderSystem->AddPass(eastl::make_unique<SE_G::ShadowMapPass>(
				renderSystem->GetDevice(), renderSystem->GetDeviceContext(),
				gPass, m_lightData)));
		m_lightTech->AssignShadowMapPass(m_shadowMapPass);
		m_lightTech->EnableShadow();
	}
}

void DirectionalLight::DisableShadow() {
	m_lightTech->DisableShadow();
}

DirectionalLight_Info::DirectionalLight_Info(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera, SE_G::DirectionalLightData initData,
	bool castsShadow)
{
	initData.Direction.Normalize();

	m_lightData = eastl::make_shared<SE_G::DirectionalLightData>(initData);
    m_name = "DirectionalLight";
    m_group = GameObjectGroup::Lighting;
	m_type.m_asLight = LightObjectType::DirectionalLight;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>(device);
	tc_info->m_assignedComponent->m_position = initData.Position;

	// RenderComponent and Passes
	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

	// LightPass - LightTechnique
	auto lightTech =
		eastl::make_unique<SE_G::DirectionalLightTechnique>(device, tc_info->m_assignedComponent.get(), "LightPass", camera, m_lightData);

	if (castsShadow)
	{
		//renderSystem->
		auto gPass = static_cast<SE_G::GPass*>(renderSystem->GetPass(SE_G::RenderPass::PassType::GPass));

		m_shadowMapPass = static_cast<SE_G::ShadowMapPass*>(
			renderSystem->AddPass(eastl::make_unique<SE_G::ShadowMapPass>(
				renderSystem->GetDevice(), renderSystem->GetDeviceContext(),
				gPass, m_lightData)));

		lightTech->AssignShadowMapPass(m_shadowMapPass);
		lightTech->EnableShadow();
	}

	m_lightTech = static_cast<SE_G::DirectionalLightTechnique*>(rc_info->AddTechnique(eastl::move(lightTech)));

	// IconPass
	auto iconTech =
		eastl::make_unique<SE_G::IconTechnique>(device, tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
			SE_G::IconData{ 2u, 0u, 1u, 1u, m_UUID.GetHilo() });
	rc_info->AddTechnique(eastl::move(iconTech));
}

DirectionalLight_Info::DirectionalLight_Info(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	const json& j)
{
	m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	m_lightData = eastl::make_shared<SE_G::DirectionalLightData>(j["m_lightData"]);
	m_name = "DirectionalLight";
	m_group = GameObjectGroup::Lighting;
	m_type.m_asLight = LightObjectType::DirectionalLight;

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
		eastl::make_unique<SE_G::DirectionalLightTechnique>(device, tc_info->m_assignedComponent.get(), "LightPass", camera, m_lightData);

	if (j.contains("CastsShadow") && j["CastsShadow"])
	{
		auto gPass = static_cast<SE_G::GPass*>(renderSystem->GetPass(SE_G::RenderPass::PassType::GPass));

		m_shadowMapPass = static_cast<SE_G::ShadowMapPass*>(
			renderSystem->AddPass(eastl::make_unique<SE_G::ShadowMapPass>(
				renderSystem->GetDevice(), renderSystem->GetDeviceContext(),
				gPass, m_lightData)));
		lightTech->AssignShadowMapPass(m_shadowMapPass);
		lightTech->EnableShadow();
	}

	m_lightTech = static_cast<SE_G::DirectionalLightTechnique*>(rc_info->AddTechnique(eastl::move(lightTech)));

	// IconPass
	auto iconTech =
		eastl::make_unique<SE_G::IconTechnique>(device, tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
			SE_G::IconData{ 2u, 0u, 1u, 1u, m_UUID.GetHilo() });
	rc_info->AddTechnique(eastl::move(iconTech));
}

void DirectionalLight_Info::EnableShadow(
	SE_G::DeferredRenderer* renderSystem) {
	if (m_shadowMapPass)
		m_lightTech->EnableShadow();
	else
	{
		auto gPass = static_cast<SE_G::GPass*>(renderSystem->GetPass(SE_G::RenderPass::PassType::GPass));

		m_shadowMapPass = static_cast<SE_G::ShadowMapPass*>(
			renderSystem->AddPass(eastl::make_unique<SE_G::ShadowMapPass>(
				renderSystem->GetDevice(), renderSystem->GetDeviceContext(),
				gPass, m_lightData)));
		m_lightTech->AssignShadowMapPass(m_shadowMapPass);
		m_lightTech->EnableShadow();
	}
}

void DirectionalLight_Info::DisableShadow() {
	m_lightTech->DisableShadow();
}
