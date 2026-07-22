#include <GameObject/EditorObjectFactory.h>

#include <GameObject/GameObject.h>
#include <GameObject/Lighting/LightCollection.h>
#include <GameObject/Shapes/ShapeCollection.h>

#include <ParticleSystem/ParticleEmitterComponent.h>
#include <ParticleSystem/ParticleSystem.h>

#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>
#include <Component/MeshComponent.h>

#include <Graphics/GraphicsResources/Texture.h>
#include <Graphics/GraphicsResources/Mesh.h>
#include <Graphics/Renderer/DeferredRenderer.h>

#include <Graphics/Renderer/Technique/GPassTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>
#include <Graphics/Renderer/Technique/EmitterTechnique.h>

#include <ResourceManager/ResourceManagerFacade.h>

#include <Graphics/Utils/Camera.h>

eastl::unique_ptr<GameObject_Info> EditorObjectFactory::CreateCustomMesh(
	SE_G::DeferredRenderer* renderSystem,
	AssetPath meshPath)
{

	auto obj = eastl::make_unique<GameObject_Info>();

	auto device = renderSystem->GetDevice();
	obj->m_group = GameObjectGroup::CustomMesh;
	obj->m_name = "CustomObject";

	// TransformComponent
	auto tc_info = obj->AddComponent<TransformComponent_Info>(device);

	// RenderComponent and techniques
	auto rc_info = obj->AddComponent<RenderComponent_Info>(obj->m_UUID, renderSystem);

	auto meshPtr = eastl::make_shared<SE_G::Mesh>(rc_info->GetDevice(), meshPath);

	auto mc_info = obj->AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), obj->m_UUID, meshPtr);

	auto& rm = ResourceManagerFacade::Instance();
	AssetPath texPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	texture->m_texturePath = texRes->m_texturePath;
	mc_info->SetTexture(texture);

	return obj;
}

eastl::unique_ptr<GameObject_Info> EditorObjectFactory::CreateCustomMesh(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{

	auto obj = eastl::make_unique<GameObject_Info>();

	auto device = renderSystem->GetDevice();
	obj->m_group = GameObjectGroup::CustomMesh;
	obj->m_name = "CustomObject";
	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());

	// TransformComponent
	auto tc_info = obj->AddComponent<TransformComponent_Info>(device);
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}

	// RenderComponent and techniques
	auto rc_info = obj->AddComponent<RenderComponent_Info>(obj->m_UUID, renderSystem);
	if (j["components"].contains("Render")) {
		rc_info->FromJson(j["components"]["Render"]);
	}

	auto mc_info = obj->AddComponent<MeshComponent_Info>();
	mc_info->FromJson(j["components"]["Mesh"],
		device, rc_info.get(),
		tc_info.get(), obj->m_UUID);

	return obj;
}

eastl::unique_ptr<BoxShapeObject_Info> EditorObjectFactory::CreateBoxObject(
	SE_G::DeferredRenderer* renderSystem)
{
	auto obj = eastl::make_unique<BoxShapeObject_Info>(renderSystem,
		BoxShapeData{ });
	return obj;
}

eastl::unique_ptr<BoxShapeObject_Info> EditorObjectFactory::CreateBoxObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = BoxShapeObject_Info::FromJson(renderSystem, j);
	return obj;
}

eastl::unique_ptr<PlaneShapeObject_Info> EditorObjectFactory::CreatePlaneObject(SE_G::DeferredRenderer* renderSystem)
{
	auto obj = eastl::make_unique<PlaneShapeObject_Info>(renderSystem,
		PlaneShapeData{ });
	return obj;
}

eastl::unique_ptr<PlaneShapeObject_Info> EditorObjectFactory::CreatePlaneObject(SE_G::DeferredRenderer* renderSystem, const json& j)
{
	auto obj = PlaneShapeObject_Info::FromJson(renderSystem, j);
	return obj;
}

eastl::unique_ptr<SphereShapeObject_Info> EditorObjectFactory::CreateSphereObject(
	SE_G::DeferredRenderer* renderSystem)
{
	auto obj = eastl::make_unique<SphereShapeObject_Info>(renderSystem,
		SphereShapeData{ 10u, 10u });
	return obj;
}

eastl::unique_ptr<SphereShapeObject_Info> EditorObjectFactory::CreateSphereObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = SphereShapeObject_Info::FromJson(renderSystem, j);
	return obj;
}

eastl::unique_ptr<GeosphereShapeObject_Info> EditorObjectFactory::CreateGeosphereObject(
	SE_G::DeferredRenderer* renderSystem)
{
	auto obj = eastl::make_unique<GeosphereShapeObject_Info>(renderSystem,
		GeosphereShapeData{ 2u });
	return obj;
}

eastl::unique_ptr<GeosphereShapeObject_Info> EditorObjectFactory::CreateGeosphereObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = GeosphereShapeObject_Info::FromJson(renderSystem, j);
	return obj;
}

eastl::unique_ptr<CylinderShapeObject_Info> EditorObjectFactory::CreateCylinderObject(
	SE_G::DeferredRenderer* renderSystem)
{
	auto obj = eastl::make_unique<CylinderShapeObject_Info>(renderSystem,
		CylinderShapeData{ 10u });
	return obj;
}

eastl::unique_ptr<CylinderShapeObject_Info> EditorObjectFactory::CreateCylinderObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = CylinderShapeObject_Info::FromJson(renderSystem, j);
	return obj;
}

eastl::unique_ptr<SkyBox_Info> EditorObjectFactory::CreateSkyBox(
	SE_G::DeferredRenderer* renderSystem,
	AssetPath assetPath,
	SE_G::SkyBoxData initData)
{
	auto obj = eastl::make_unique<SkyBox_Info>(renderSystem, assetPath, initData);
	return obj;
}

eastl::unique_ptr<AmbientLight_Info> EditorObjectFactory::CreateAmbientLightObject(
	SE_G::DeferredRenderer* renderSystem,
	SE_G::AmbientLightData initData)
{
	auto obj = eastl::make_unique<AmbientLight_Info>(renderSystem, initData);
	return obj;
}

eastl::unique_ptr<DirectionalLight_Info> EditorObjectFactory::CreateDirectionalLightObject(
	SE_G::DeferredRenderer* renderSystem,
	SE_G::DirectionalLightData initData)
{
	auto obj = eastl::make_unique<DirectionalLight_Info>(renderSystem, initData, true);
	return obj;
}

eastl::unique_ptr<PointLight_Info> EditorObjectFactory::CreatePointLightObject(
	SE_G::DeferredRenderer* renderSystem,
	SE_G::PointLightData initData)
{
	auto obj = eastl::make_unique<PointLight_Info>(renderSystem, initData);
	return obj;
}

eastl::unique_ptr<SpotLight_Info> EditorObjectFactory::CreateSpotLightObject(
	SE_G::DeferredRenderer* renderSystem,
	SE_G::SpotLightData initData)
{
	auto obj = eastl::make_unique<SpotLight_Info>(renderSystem, initData);
	return obj;
}
