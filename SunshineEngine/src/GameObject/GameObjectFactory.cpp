#include <GameObject/GameObjectFactory.h>
#include <GameObject/GameObject.h>
#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>
#include <Component/MeshComponent.h>
#include <GameObject/Lighting/AmbientLight.h>
#include <GameObject/Lighting/DirectionalLight.h>
#include <GameObject/Lighting/PointLight.h>
#include <GameObject/Lighting/SpotLight.h>
#include <GameObject/Lighting/SkyBox.h>
#include <GameObject/Shapes/ShapeData.h>

#include <Graphics/GraphicsResources/Mesh.h>

#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/Technique/GPassTechnique.h>
#include <Graphics/Renderer/Technique/LightTechnique.h>
#include <Graphics/Renderer/Technique/AmbientLightTechnique.h>
#include <Graphics/Renderer/Technique/DirectionalLightTechnique.h>
#include <Graphics/Renderer/Technique/PointLightTechnique.h>
#include <Graphics/Renderer/Technique/SpotLightTechnique.h>
#include <Graphics/Renderer/Technique/SkyBoxTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>

#include <Serialization/ShapeSerialization.h>

#include <ResourceManager/ResourceManagerFacade.h>

eastl::unique_ptr<GameObject> GameObjectFactory::CreateCustomMesh(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = eastl::make_unique<GameObject>();
	obj->m_name = "CustomObject";

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = obj->AddComponent<TransformComponent>(device);
	if (j["components"].contains("Transform")) {
		tc->FromJson(j["components"]["Transform"]);
	}

	// RenderComponent and technique
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	auto mc = obj->AddComponent<MeshComponent>();
	mc->FromJson(j["components"]["Mesh"], device, rc.get(), tc.get(), obj->m_UUID);

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateCustomMesh(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Mesh> mesh)
{
	auto obj = eastl::make_unique<GameObject>();
	obj->m_name = "CustomObject";

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = obj->AddComponent<TransformComponent>(device);

	// RenderComponent and technique
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, mesh);

	/*
	auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc->GetDevice(),
		SE_G::Colors::UnloadedTextureColor, 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
	*/
	auto& rm = ResourceManagerFacade::Instance();
	AssetPath texPath(L"UnloadedTextureColor.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	if (texRes)
	{
		auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
			texRes,
			[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
		//mc_info->SetTexture(texture);
		mc->SetTexture(texture);
	}
	else
	{
		auto texture = eastl::make_shared<SE_G::Bind::Texture>(
			renderSystem->GetDevice(),
			texPath,
			0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);
		//mc_info->SetTexture(texture);
		mc->SetTexture(texture);
	}


	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateCustomMesh(
	SE_G::DeferredRenderer* renderSystem,
	AssetPath meshPath)
{
	auto obj = eastl::make_unique<GameObject>();
	obj->m_name = "CustomObject";

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = obj->AddComponent<TransformComponent>(device);

	// RenderComponent and technique
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	auto meshPtr = eastl::make_shared<SE_G::Mesh>(rc->GetDevice(), meshPath);
	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	/*
	auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc->GetDevice(),
		AssetPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine), 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
	*/
	auto& rm = ResourceManagerFacade::Instance();
	AssetPath texPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	if (texRes)
	{
		auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
			texRes,
			[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
		//mc_info->SetTexture(texture);
		mc->SetTexture(texture);
	}
	else
	{
		auto texture = eastl::make_shared<SE_G::Bind::Texture>(
			renderSystem->GetDevice(),
			texPath,
			0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);
		//mc_info->SetTexture(texture);
		mc->SetTexture(texture);
	}

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateBoxObject(
	SE_G::DeferredRenderer* renderSystem,
	float width, float height, float length)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject>();
	auto tc = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	auto meshPtr = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(device, DXSM::Vector3(width, height, length));
	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	/*
	auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc->GetDevice(),
		AssetPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine), 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
	*/
	auto& rm = ResourceManagerFacade::Instance();
	AssetPath texPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	if (texRes)
	{
		auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
			texRes,
			[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
		//mc_info->SetTexture(texture);
		mc->SetTexture(texture);
	}
	else
	{
		auto texture = eastl::make_shared<SE_G::Bind::Texture>(
			renderSystem->GetDevice(),
			texPath,
			0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);
		//mc_info->SetTexture(texture);
		mc->SetTexture(texture);
	}

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateBoxObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	eastl::unique_ptr<GameObject> obj = eastl::make_unique<GameObject>();

	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	obj->m_name = "Box";

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = obj->AddComponent<TransformComponent>(device);
	if (j["components"].contains("Transform")) {
		tc->FromJson(j["components"]["Transform"]);
	}

	// RenderComponent and technique
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	auto shapeData = eastl::make_shared<BoxShapeData>(j["m_shapeData"].get<BoxShapeData>());
	auto meshPtr = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(device, shapeData->Size);
	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	AssetPath texPath;
	if (j["components"]["Mesh"].contains("Texture"))
	{
		texPath.FromJson(j["components"]["Mesh"]["Texture"]);
	}
	else {
		texPath = AssetPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
	}
	/*
	auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		device, texPath, 0u, SE_G::Bind::PipelineStage::PIXEL_SHADER);
	*/
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	if (texRes)
	{
		auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
			texRes,
			[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
		mc->SetTexture(texture);
	}
	else
	{
		auto texture = eastl::make_shared<SE_G::Bind::Texture>(
			renderSystem->GetDevice(),
			texPath,
			0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);
		mc->SetTexture(texture);
	}

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateSphereObject(
	SE_G::DeferredRenderer* renderSystem, float radius)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject>();
	auto tc = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	auto meshPtr = SE_G::Mesh::CreateSphereMesh(device, DXSM::Vector3::One * radius);
	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	/*auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc->GetDevice(),
		AssetPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine), 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
	mc->SetTexture(texture);*/

	auto& rm = ResourceManagerFacade::Instance();
	AssetPath texPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	if (texRes)
	{
		auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
			texRes,
			[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
		mc->SetTexture(texture);
	}
	else
	{
		auto texture = eastl::make_shared<SE_G::Bind::Texture>(
			rc->GetDevice(), texPath, 0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);
		mc->SetTexture(texture);
	}

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateSphereObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	eastl::unique_ptr<GameObject> obj = eastl::make_unique<GameObject>();

	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	obj->m_name = "Sphere";

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = obj->AddComponent<TransformComponent>(device);
	if (j["components"].contains("Transform")) {
		tc->FromJson(j["components"]["Transform"]);
	}

	// RenderComponent and technique
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	auto shapeData = eastl::make_shared<SphereShapeData>(j["m_shapeData"].get<SphereShapeData>());
	auto meshPtr = SE_G::Mesh::CreateSphereMesh(device, shapeData->Size, shapeData->SliceCount, shapeData->StackCount);
	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	AssetPath texPath;
	if (j["components"]["Mesh"].contains("Texture"))
	{
		texPath.FromJson(j["components"]["Mesh"]["Texture"]);
	}
	else {
		texPath = AssetPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine);
	}
	/*
	auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		device, texPath, 0u, SE_G::Bind::PipelineStage::PIXEL_SHADER);
	mc->SetTexture(texture);
	*/
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	if (texRes)
	{
		auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
			texRes,
			[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
		mc->SetTexture(texture);
	}
	else
	{
		auto texture = eastl::make_shared<SE_G::Bind::Texture>(
			rc->GetDevice(), texPath, 0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);
		mc->SetTexture(texture);
	}

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateGeosphereObject(
	SE_G::DeferredRenderer* renderSystem, float radius)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject>();
	auto tc = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	auto meshPtr = SE_G::Mesh::CreateGeosphereMesh(device, DXSM::Vector3::One * radius, 2u);
	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	/*auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc->GetDevice(),
		AssetPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine), 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
	mc->SetTexture(texture);*/

	auto& rm = ResourceManagerFacade::Instance();
	AssetPath texPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	if (texRes)
	{
		auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
			texRes,
			[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
		mc->SetTexture(texture);
	}
	else
	{
		auto texture = eastl::make_shared<SE_G::Bind::Texture>(
			rc->GetDevice(),
			texPath,
			0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);
		mc->SetTexture(texture);
	}

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateGeosphereObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	eastl::unique_ptr<GameObject> obj = eastl::make_unique<GameObject>();

	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	obj->m_name = "Sphere";

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = obj->AddComponent<TransformComponent>(device);
	if (j["components"].contains("Transform")) {
		tc->FromJson(j["components"]["Transform"]);
	}

	// RenderComponent and technique
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	auto shapeData = eastl::make_shared<GeosphereShapeData>(j["m_shapeData"].get<GeosphereShapeData>());
	auto meshPtr = SE_G::Mesh::CreateGeosphereMesh(device, shapeData->Size, shapeData->NumSubdivisions);
	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	AssetPath texPath;
	if (j["components"]["Mesh"].contains("Texture"))
	{
		texPath.FromJson(j["components"]["Mesh"]["Texture"]);
	}
	else {
		texPath = AssetPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine);
	}

	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	if (texRes)
	{
		auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
			texRes,
			[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
		mc->SetTexture(texture);
	}
	else
	{
		auto texture = eastl::make_shared<SE_G::Bind::Texture>(
			rc->GetDevice(),
			texPath,
			0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);
		mc->SetTexture(texture);
	}

	return obj;
}

eastl::unique_ptr<SkyBox> GameObjectFactory::CreateSkyBox(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	AssetPath texturePath,
	SE_G::SkyBoxData initData)
{
	auto obj = eastl::make_unique<SkyBox>(renderSystem, camera, texturePath, initData);
	return obj;
}

eastl::unique_ptr<SkyBox> GameObjectFactory::CreateSkyBox(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	const json& j)
{
	auto obj = eastl::make_unique<SkyBox>(renderSystem, camera, j);
	return obj;
}

eastl::unique_ptr<AmbientLight> GameObjectFactory::CreateAmbientLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::AmbientLightData initData)
{
	auto obj = eastl::make_unique<AmbientLight>(renderSystem, camera, initData);
	return obj;
}

eastl::unique_ptr<AmbientLight> GameObjectFactory::CreateAmbientLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	const json& j)
{
	auto obj = eastl::make_unique<AmbientLight>(renderSystem, camera, j);
	return obj;
}

eastl::unique_ptr<DirectionalLight> GameObjectFactory::CreateDirectionalLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::DirectionalLightData initData)
{
	auto obj = eastl::make_unique<DirectionalLight>(renderSystem, camera, initData);
	return obj;
}

eastl::unique_ptr<DirectionalLight> GameObjectFactory::CreateDirectionalLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	const json& j)
{
	auto obj = eastl::make_unique<DirectionalLight>(renderSystem, camera, j);
	return obj;
}

eastl::unique_ptr<PointLight> GameObjectFactory::CreatePointLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::PointLightData initData)
{
	auto obj = eastl::make_unique<PointLight>(renderSystem, camera, initData);
	return obj;
}

eastl::unique_ptr<PointLight> GameObjectFactory::CreatePointLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	const json& j)
{
	auto obj = eastl::make_unique<PointLight>(renderSystem, camera, j);
	return obj;
}

eastl::unique_ptr<SpotLight> GameObjectFactory::CreateSpotLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::SpotLightData initData)
{
	auto obj = eastl::make_unique<SpotLight>(renderSystem, camera, initData);
	return obj;
}

eastl::unique_ptr<SpotLight> GameObjectFactory::CreateSpotLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	const json& j)
{
	auto obj = eastl::make_unique<SpotLight>(renderSystem, camera, j);
	return obj;
}
