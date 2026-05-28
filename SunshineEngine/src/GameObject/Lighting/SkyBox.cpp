#include <GameObject/Lighting/SkyBox.h>

#include <Graphics/Renderer/Technique/SkyBoxTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>
#include <Graphics/Renderer/DeferredRenderer.h>

#include <Graphics/GraphicsResources/Texture.h>

#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>



SkyBox::SkyBox(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	AssetPath assetPath,
	SE_G::SkyBoxData initData)
{
	m_lightData = eastl::make_shared<SE_G::SkyBoxData>(initData);

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = eastl::make_shared<TransformComponent>(device);

	// RenderComponent and Passes
	auto rc = eastl::make_shared<RenderComponent>(m_UUID, renderSystem);

	// LightPass - LightTechnique
	auto lightTech =
		eastl::make_unique<SE_G::SkyBoxTechnique>(
			device, tc.get(), "LightPass",
			camera, m_lightData, assetPath);
	rc->AddTechnique(eastl::move(lightTech));

	// IconPass
	auto iconTech =
		eastl::make_unique<SE_G::IconTechnique>(
			device, tc.get(), eastl::string("IconPass"),
			SE_G::IconData{ 0u, 0u, 1u, 1u, m_UUID.GetHilo() });
	rc->AddTechnique(eastl::move(iconTech));
}

SkyBox::SkyBox(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	const json& j)
{
	m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	m_lightData = eastl::make_shared<SE_G::SkyBoxData>(j["m_lightData"]);
	m_name = "SkyBox";

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = AddComponent<TransformComponent>(device);
	if (j["components"].contains("Transform")) {
		tc->FromJson(j["components"]["Transform"]);
	}

	// RenderComponent and Passes
	auto rc = AddComponent<RenderComponent>(m_UUID, renderSystem);

	// LightPass - LightTechnique
	auto lightTech =
		eastl::make_unique<SE_G::SkyBoxTechnique>(
			device, tc.get(), "LightPass", camera, m_lightData);
	rc->AddTechnique(eastl::move(lightTech));
}


SkyBox_Info::SkyBox_Info(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	AssetPath assetPath,
	SE_G::SkyBoxData initData)
{
	m_lightData = eastl::make_shared<SE_G::SkyBoxData>(initData);
	m_name = "SkyBox";
	m_group = GameObjectGroup::Lighting;
	m_type.m_asLight = LightObjectType::SkyBox;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>(device);

	// RenderComponent and Passes
	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

	// LightPass - LightTechnique
	auto lightTech =
		eastl::make_unique<SE_G::SkyBoxTechnique>(
			device, tc_info->m_assignedComponent.get(), "LightPass",
			camera, m_lightData, assetPath);
	m_lightTech = static_cast<SE_G::SkyBoxTechnique*>(rc_info->AddTechnique(eastl::move(lightTech)));

	// IconPass
	auto iconTech = eastl::make_unique<SE_G::IconTechnique>(
		device, tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
		SE_G::IconData{ 0u, 0u, 1u, 1u, m_UUID.GetHilo() });
	rc_info->AddTechnique(eastl::move(iconTech));
}

SkyBox_Info::SkyBox_Info(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	const json& j)
{
	m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	m_lightData = eastl::make_shared<SE_G::SkyBoxData>(j["m_lightData"]);
	m_name = "SkyBox";
	m_group = GameObjectGroup::Lighting;
	m_type.m_asLight = LightObjectType::SkyBox;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>(device);
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}

	// RenderComponent and Passes
	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

	// LightPass - LightTechnique
	auto lightTech =
		eastl::make_unique<SE_G::SkyBoxTechnique>(
			device, tc_info->m_assignedComponent.get(), "LightPass",
			camera, m_lightData); // , texturePath);
	m_lightTech = static_cast<SE_G::SkyBoxTechnique*>(rc_info->AddTechnique(eastl::move(lightTech)));

	// IconPass
	auto iconTech = eastl::make_unique<SE_G::IconTechnique>(
		device, tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
		SE_G::IconData{ 0u, 0u, 1u, 1u, m_UUID.GetHilo() });
	rc_info->AddTechnique(eastl::move(iconTech));
}

void SkyBox_Info::SetTexture(eastl::shared_ptr<SE_G::Bind::Texture> tex)
{
	m_lightTech->m_texture = tex;
}

void SkyBox_Info::SetTexture(SE_G::DeferredRenderer* renderSystem, AssetPath assetPath)
{
	auto device = renderSystem->GetDevice();
	auto tex = eastl::make_shared<SE_G::Bind::Texture>(device, assetPath, 0u, SE_G::Bind::PipelineStage::PIXEL_SHADER);

	m_lightTech->m_texture = tex;
}