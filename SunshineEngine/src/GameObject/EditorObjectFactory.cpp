#include <GameObject/EditorObjectFactory.h>
#include <GameObject/GameObject.h>
#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>

#include <Graphics/Renderer/DeferredRenderer.h>

#include <GameObject/Lighting/AmbientLight.h>
#include <GameObject/Lighting/PointLight.h>
#include <GameObject/Lighting/DirectionalLight.h>
#include <GameObject/Lighting/SkyBox.h>

#include <Graphics/Renderer/Technique/GPassTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>

eastl::unique_ptr<GameObject_Info> EditorObjectFactory::CreateDefaultBoxObject(
	SE_G::DeferredRenderer* renderSystem,
	float width, float height, float length)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject_Info>();
	obj->m_group = GameObjectGroup::Shapes;
	obj->m_type.m_asShape = ShapeObjectType::Box;
	obj->m_name = "Box";

	// TransformComponent
	auto tc_info = obj->AddComponent<TransformComponent_Info>();
	tc_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);

	// RenderComponent and Passes
	// Need:
	// 1) Outside render json: transformComp, UUID
	// 2) Inside render json: mesh path, texture path
	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);

	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
		renderSystem, tc_info->m_assignedComponent.get(), "GPass", obj->m_UUID);
	gBufferTech->m_mesh = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(device, DXSM::Vector3(width, height, length));
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultTexture.dds"));

	rc_info->AddTechnique(eastl::move(gBufferTech));

	return eastl::move(obj);
}

eastl::unique_ptr<GameObject_Info> EditorObjectFactory::CreateDefaultBoxObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject_Info>();
	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	obj->m_group = GameObjectGroup::Shapes;
	obj->m_type.m_asShape = ShapeObjectType::Box;
	obj->m_name = "Box";

	// TransformComponent
	auto tc_info = obj->AddComponent<TransformComponent_Info>();
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}
	else {
		tc_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);
	}

	// RenderComponent and Passes
	// Need:
	// 1) Outside render json: transformComp, UUID
	// 2) Inside render json: mesh path, texture path
	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);

	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
		renderSystem, tc_info->m_assignedComponent.get(), "GPass", obj->m_UUID);
	gBufferTech->m_mesh = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(device, DXSM::Vector3::One);
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultTexture.dds"));

	rc_info->AddTechnique(eastl::move(gBufferTech));

	return eastl::move(obj);
}

eastl::unique_ptr<GameObject_Info> EditorObjectFactory::CreateDefaultSphereObject(
	SE_G::DeferredRenderer* renderSystem, float radius)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject_Info>();
	obj->m_group = GameObjectGroup::Shapes;
	obj->m_type.m_asShape = ShapeObjectType::Sphere;
	obj->m_name = "Sphere";

	// TransformComponent
	auto tc_info = obj->AddComponent<TransformComponent_Info>();
	tc_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);

	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);

	// RenderComponent and Passes
	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
		renderSystem, tc_info->m_assignedComponent.get(), "GPass", obj->m_UUID);
	gBufferTech->m_mesh = SE_G::Mesh::CreateSphereMesh(device, DXSM::Vector3::One * radius);
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"));
	rc_info->AddTechnique(eastl::move(gBufferTech));

	return eastl::move(obj);
}

eastl::unique_ptr<GameObject_Info> EditorObjectFactory::CreateDefaultSphereObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject_Info>();
	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	obj->m_group = GameObjectGroup::Shapes;
	obj->m_type.m_asShape = ShapeObjectType::Sphere;
	obj->m_name = "Sphere";

	// TransformComponent
	auto tc_info = obj->AddComponent<TransformComponent_Info>();
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}
	else {
		tc_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);
	}

	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);

	// RenderComponent and Passes
	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
		renderSystem, tc_info->m_assignedComponent.get(), "GPass", obj->m_UUID);
	gBufferTech->m_mesh = SE_G::Mesh::CreateSphereMesh(device, DXSM::Vector3::One);
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"));
	rc_info->AddTechnique(eastl::move(gBufferTech));

	return eastl::move(obj);
}

eastl::unique_ptr<SkyBox_Info> EditorObjectFactory::CreateSkyBox(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	eastl::wstring texturePath,
	SE_G::SkyBoxData initData)
{
	auto obj = eastl::make_unique<SkyBox_Info>(renderSystem, camera, texturePath, initData);
	return eastl::move(obj);
}

eastl::unique_ptr<AmbientLight_Info> EditorObjectFactory::CreateAmbientLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::AmbientLightData initData)
{
	auto obj = eastl::make_unique<AmbientLight_Info>(renderSystem, camera, initData);
	return eastl::move(obj);
}

eastl::unique_ptr<DirectionalLight_Info> EditorObjectFactory::CreateDirectionalLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::DirectionalLightData initData)
{
	auto obj = eastl::make_unique<DirectionalLight_Info>(renderSystem, camera, initData);
	return eastl::move(obj);
}

eastl::unique_ptr<PointLight_Info> EditorObjectFactory::CreatePointLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::PointLightData initData)
{
	auto obj = eastl::make_unique<PointLight_Info>(renderSystem, camera, initData);
	return eastl::move(obj);
}


