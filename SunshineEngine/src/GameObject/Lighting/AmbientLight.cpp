#include <GameObject/Lighting/AmbientLight.h>

#include <Graphics/Renderer/Technique/AmbientLightTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>
#include <Graphics/Renderer/DeferredRenderer.h>

#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>

#include <Serialization/LightDataSerialization.h>

AmbientLight::AmbientLight(
	SE_G::DeferredRenderer* renderSystem,
	SE_G::AmbientLightData initData)
{
    m_lightData = eastl::make_shared<SE_G::AmbientLightData>(initData);

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = eastl::make_shared<TransformComponent>(device);

	// RenderComponent and Passes
	// Need:
	// 1) Outside render json: transformComp, UUID, lightData, camera

	// LightPass - LightTechnique
	auto lightTech = eastl::make_unique<SE_G::AmbientLightTechnique>(
		renderSystem, tc.get(),
		"LightPass", m_lightData);

	auto rc = eastl::make_shared<RenderComponent>(m_UUID, renderSystem);
	rc->AddTechnique(eastl::move(lightTech));
}

AmbientLight::AmbientLight(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	m_lightData = eastl::make_shared<SE_G::AmbientLightData>(j["m_lightData"]);
	m_name = "AmbientLight";

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = AddComponent<TransformComponent>(device);
	if (j["components"].contains("Transform")) {
		tc->FromJson(j["components"]["Transform"]);
	}

	// RenderComponent and Passes
	auto rc = AddComponent<RenderComponent>(m_UUID, renderSystem);

	// LightPass - LightTechnique
	auto lightTech = eastl::make_unique<SE_G::AmbientLightTechnique>(
		renderSystem, tc.get(), "LightPass", m_lightData);
	rc->AddTechnique(eastl::move(lightTech));

}


AmbientLight_Info::AmbientLight_Info(
    SE_G::DeferredRenderer* renderSystem,
    SE_G::AmbientLightData initData)
{
    m_lightData = eastl::make_shared<SE_G::AmbientLightData>(initData);
    m_name = "AmbientLight";
    m_group = GameObjectGroup::Lighting;
	m_type.m_asLight = LightObjectType::AmbientLight;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>(device);

	// RenderComponent and Passes
	// Need:
	// 1) Outside render json: transformComp, UUID, lightData, camera

	// LightPass - LightTechnique
	auto lightTech = eastl::make_unique<SE_G::AmbientLightTechnique>(
		renderSystem, tc_info->m_assignedComponent.get(),
		"LightPass", m_lightData);

	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);
	rc_info->AddTechnique(eastl::move(lightTech));

	// IconPass
	auto iconTech = eastl::make_unique<SE_G::IconTechnique>(device, tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
		SE_G::IconData{ 1u, 0u, 1u, 1u, m_UUID.GetHilo() });

	rc_info->AddTechnique(eastl::move(iconTech));

}

AmbientLight_Info::AmbientLight_Info(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	m_lightData = eastl::make_shared<SE_G::AmbientLightData>(j["m_lightData"]);
	m_name = "AmbientLight";
	m_group = GameObjectGroup::Lighting;
	m_type.m_asLight = LightObjectType::AmbientLight;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>(device);
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}

	// RenderComponent and Passes
	// Need:
	// 1) Outside render json: transformComp, UUID, lightData, camera

	// LightPass - LightTechnique
	auto lightTech = eastl::make_unique<SE_G::AmbientLightTechnique>(
		renderSystem, tc_info->m_assignedComponent.get(),
		"LightPass", m_lightData);

	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);
	rc_info->AddTechnique(eastl::move(lightTech));

	// IconPass
	auto iconTech = eastl::make_unique<SE_G::IconTechnique>(device, tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
		SE_G::IconData{ 1u, 0u, 1u, 1u, m_UUID.GetHilo() });

	rc_info->AddTechnique(eastl::move(iconTech));

}
