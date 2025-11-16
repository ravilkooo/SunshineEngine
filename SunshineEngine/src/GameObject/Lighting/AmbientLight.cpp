#include <GameObject/Lighting/AmbientLight.h>

#include <Graphics/Renderer/Technique/AmbientLightTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>
#include <Graphics/Renderer/DeferredRenderer.h>

#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>



AmbientLight::AmbientLight(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
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
		device, tc.get(),
		"LightPass", camera, m_lightData);

	auto rc = eastl::make_shared<RenderComponent>(renderSystem);
	rc->AddTechnique(eastl::move(lightTech));

	// IconPass
	auto iconTech = eastl::make_unique<SE_G::IconTechnique>(device, tc.get(), eastl::string("IconPass"),
		SE_G::IconData{ 1u, 0u, 1u, 1u, m_UUID.GetHilo() });
	rc->AddTechnique(eastl::move(iconTech));
}

AmbientLight_Info::AmbientLight_Info(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    SE_G::AmbientLightData initData)
{
    m_lightData = eastl::make_shared<SE_G::AmbientLightData>(initData);
    m_name = "AmbientLight";
    m_group = GameObjectGroup::Lighting;
	m_type.m_asLight = LightObjectType::AmbientLight;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>();
	tc_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);

	// RenderComponent and Passes
	// Need:
	// 1) Outside render json: transformComp, UUID, lightData, camera

	// LightPass - LightTechnique
	auto lightTech = eastl::make_unique<SE_G::AmbientLightTechnique>(
		device, tc_info->m_assignedComponent.get(),
		"LightPass", camera, m_lightData);

	auto rc_info = AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);
	rc_info->AddTechnique(eastl::move(lightTech));

	// IconPass
	auto iconTech = eastl::make_unique<SE_G::IconTechnique>(device, tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
		SE_G::IconData{ 1u, 0u, 1u, 1u, m_UUID.GetHilo() });

	rc_info->AddTechnique(eastl::move(iconTech));

}

AmbientLight_Info::AmbientLight_Info(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	const json& j)
{
	m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	m_lightData = eastl::make_shared<SE_G::AmbientLightData>(j["m_lightData"]);
	m_name = "AmbientLight";
	m_group = GameObjectGroup::Lighting;
	m_type.m_asLight = LightObjectType::AmbientLight;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>();
	if (j["components"].contains("Transform")) {
		//tc_info->m_assignedComponent = eastl::make_shared<TransformComponent>(renderSystem->GetDevice());
		tc_info->FromJson(j["components"]["Transform"], device);
	}
	else {
		tc_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);
	}

	// RenderComponent and Passes
	// Need:
	// 1) Outside render json: transformComp, UUID, lightData, camera

	// LightPass - LightTechnique
	auto lightTech = eastl::make_unique<SE_G::AmbientLightTechnique>(
		device, tc_info->m_assignedComponent.get(),
		"LightPass", camera, m_lightData);

	auto rc_info = AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);
	rc_info->AddTechnique(eastl::move(lightTech));

	// IconPass
	auto iconTech = eastl::make_unique<SE_G::IconTechnique>(device, tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
		SE_G::IconData{ 1u, 0u, 1u, 1u, m_UUID.GetHilo() });

	rc_info->AddTechnique(eastl::move(iconTech));

}
