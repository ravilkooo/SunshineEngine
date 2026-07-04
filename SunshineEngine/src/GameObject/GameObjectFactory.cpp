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

#include <Graphics/Utils/Camera.h>

#include <ParticleSystem/ParticleEmitterComponent.h>
#include <ParticleSystem/ParticleSystem.h>

#include <Serialization/ShapeSerialization.h>

#include <ResourceManager/ResourceManagerFacade.h>


eastl::unique_ptr<GameObject> GameObjectFactory::CreateParticleEmitter(
	SE::ParticleSystem* particleSystem)
{
	auto device = particleSystem->m_renderer->GetDevice();
	auto obj = eastl::make_unique<GameObject>();

	// TransformComponent
	auto tc = obj->AddComponent<TransformComponent>(device);

	// RenderComponent
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, particleSystem->m_renderer);

	// ParticleEmitterComponent
	SE::ParticleData::EmitterPointConstantBuffer emitterDesc;
	SE::ParticleData::SimulateParticlesConstantBuffer simulatorDesc;

	emitterDesc =
	{
		DXSM::Matrix::Identity,
		{ 0, 0, 0 }, 3.0f,
		{ 1, 1, 1 }, 1.0f,
		{ 1, 1, 1 }, 1.0f,

		8, 1, 0.2, 0.5,

		0, DX::XM_2PI, -DX::XM_PIDIV2, DX::XM_PIDIV2,

		100u, { 0, 0, 0 },
	};
	simulatorDesc = {
		{ 0, 0, 0 }, 0
	};

	auto pec = obj->AddComponent<ParticleEmitterComponent>(
		obj->m_UUID, tc.get(),
		particleSystem,
		emitterDesc,
		simulatorDesc);

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateParticleEmitter(
	SE::ParticleSystem* particleSystem,
	const json& j)
{
	auto device = particleSystem->m_renderer->GetDevice();
	auto obj = eastl::make_unique<GameObject>();
	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());

	// TransformComponent
	auto tc = obj->AddComponent<TransformComponent>(device);
	if (j["components"].contains("Transform")) {
		tc->FromJson(j["components"]["Transform"]);
	}

	// RenderComponent
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, particleSystem->m_renderer);
	if (j["components"].contains("Render")) {
		rc->FromJson(j["components"]["Render"]);
	}

	// ParticleEmitterComponent
	if (j["components"].contains("ParticleEmitter"))
	{
		auto pec = obj->AddComponent<ParticleEmitterComponent>();
		pec->FromJson(j["components"]["ParticleEmitter"],
			obj->m_UUID, tc.get(),
			particleSystem);
	}
	else
	{
		SE::ParticleData::EmitterPointConstantBuffer emitterDesc;
		SE::ParticleData::SimulateParticlesConstantBuffer simulatorDesc;
		emitterDesc =
		{
			DXSM::Matrix::Identity,
			{ 0, 0, 0 }, 3.0f,
			{ 1, 1, 1 }, 1.0f,
			{ 1, 1, 1 }, 1.0f,
			8, 1, 0.2, 0.5,
			0, DX::XM_2PI, -DX::XM_PIDIV2, DX::XM_PIDIV2,
			100u, { 0, 0, 0 },
		};
		simulatorDesc = {
			{ 0, 0, 0 }, 0
		};
		auto pec = obj->AddComponent<ParticleEmitterComponent>(
			obj->m_UUID, tc.get(),
			particleSystem,
			emitterDesc,
			simulatorDesc);
	}

	// MeshComponent
	if (j["components"].contains("Mesh"))
	{
		auto mc = obj->AddComponent<MeshComponent>();
		mc->FromJson(j["components"]["Mesh"], device, rc.get(), tc.get(), obj->m_UUID);
	}

	return obj;
}


eastl::unique_ptr<GameObject> GameObjectFactory::CreateCustomMesh(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = eastl::make_unique<GameObject>();
	obj->m_name = "CustomObject";
	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = obj->AddComponent<TransformComponent>(device);
	if (j["components"].contains("Transform")) {
		tc->FromJson(j["components"]["Transform"]);
	}

	// RenderComponent and technique
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);
	if (j["components"].contains("Render")) {
		rc->FromJson(j["components"]["Render"]);
	}

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

	auto& rm = ResourceManagerFacade::Instance();
	AssetPath texPath(L"UnloadedTextureColor.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	if (texHandle.guid == 0) {
		// Error
		auto ap = AssetPath(
			SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
			AssetPath::AssetSource::Engine);
		texHandle = ResourceManagerFacade::Instance().LoadByPath(ap);
	}
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	//mc_info->SetTexture(texture);
	mc->SetTexture(texture);

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

	auto& rm = ResourceManagerFacade::Instance();
	AssetPath texPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	//mc_info->SetTexture(texture);
	mc->SetTexture(texture);

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateBoxObject(
	SE_G::DeferredRenderer* renderSystem)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject>();
	auto tc = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	eastl::shared_ptr<SE_G::Mesh> meshPtr;
	AssetPath meshPath = AssetPath(L"Box_repeat");
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	meshPtr = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	meshPtr->m_meshPath = meshRes->m_meshPath;

	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	AssetPath texPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	//mc_info->SetTexture(texture);
	mc->SetTexture(texture);

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
	if (j["components"].contains("Render")) {
		rc->FromJson(j["components"]["Render"]);
	}

	auto shapeData = eastl::make_shared<BoxShapeData>(j["m_shapeData"].get<BoxShapeData>());

	eastl::shared_ptr<SE_G::Mesh> meshPtr;
	AssetPath meshPath = shapeData->UvCubeMapMode ? AssetPath(L"Box") : AssetPath(L"Box_repeat");
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	meshPtr = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	meshPtr->m_meshPath = meshRes->m_meshPath;

	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	AssetPath texPath(L"");
	if (j["components"]["Mesh"].contains("Texture"))
	{
		texPath.FromJson(j["components"]["Mesh"]["Texture"]);
	}
	else {
		texPath = AssetPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
	}

	ResourceHandle texHandle = rm.LoadByPath(texPath);
	if (texHandle.guid == 0) {
		// Error
		auto ap = AssetPath(
			SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
			AssetPath::AssetSource::Engine);
		texHandle = rm.LoadByPath(ap);
	}
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	mc->SetTexture(texture);

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreatePlaneObject(
	SE_G::DeferredRenderer* renderSystem)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject>();
	auto tc = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	eastl::shared_ptr<SE_G::Mesh> meshPtr;
	AssetPath meshPath = AssetPath(L"Plane");
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	meshPtr = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	meshPtr->m_meshPath = meshRes->m_meshPath;

	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	AssetPath texPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	//mc_info->SetTexture(texture);
	mc->SetTexture(texture);

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreatePlaneObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	eastl::unique_ptr<GameObject> obj = eastl::make_unique<GameObject>();

	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	obj->m_name = "Plane";

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = obj->AddComponent<TransformComponent>(device);
	if (j["components"].contains("Transform")) {
		tc->FromJson(j["components"]["Transform"]);
	}

	// RenderComponent and technique
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);
	if (j["components"].contains("Render")) {
		rc->FromJson(j["components"]["Render"]);
	}

	auto shapeData = eastl::make_shared<PlaneShapeData>(j["m_shapeData"].get<PlaneShapeData>());
	
	eastl::shared_ptr<SE_G::Mesh> meshPtr;
	AssetPath meshPath = AssetPath(L"Plane");
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	meshPtr = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	meshPtr->m_meshPath = meshRes->m_meshPath;

	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	AssetPath texPath(L"");
	if (j["components"]["Mesh"].contains("Texture"))
	{
		texPath.FromJson(j["components"]["Mesh"]["Texture"]);
	}
	else {
		texPath = AssetPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
	}
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	if (texHandle.guid == 0) {
		// Error
		auto ap = AssetPath(
			SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
			AssetPath::AssetSource::Engine);
		texHandle = rm.LoadByPath(ap);
	}
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	mc->SetTexture(texture);

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateSphereObject(
	SE_G::DeferredRenderer* renderSystem)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject>();
	auto tc = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	eastl::shared_ptr<SE_G::Mesh> meshPtr;
	AssetPath meshPath = AssetPath(L"Sphere");
	meshPath.m_params.asMesh.param1 = 10; // shapeData->SliceCount;
	meshPath.m_params.asMesh.param2 = 20; // shapeData->StackCount;
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	meshPtr = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	meshPtr->m_meshPath = meshRes->m_meshPath;

	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	/*auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc->GetDevice(),
		AssetPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine), 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
	mc->SetTexture(texture);*/

	AssetPath texPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	mc->SetTexture(texture);

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
	if (j["components"].contains("Render")) {
		rc->FromJson(j["components"]["Render"]);
	}

	auto shapeData = eastl::make_shared<SphereShapeData>(j["m_shapeData"].get<SphereShapeData>());

	eastl::shared_ptr<SE_G::Mesh> meshPtr;
	AssetPath meshPath = AssetPath(L"Sphere");
	meshPath.m_params.asMesh.param1 = shapeData->SliceCount;
	meshPath.m_params.asMesh.param2 = shapeData->StackCount;
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	meshPtr = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	meshPtr->m_meshPath = meshRes->m_meshPath;

	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	AssetPath texPath(L"");
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
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	if (texHandle.guid == 0) {
		// Error
		auto ap = AssetPath(
			SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
			AssetPath::AssetSource::Engine);
		texHandle = rm.LoadByPath(ap);
	}
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	mc->SetTexture(texture);

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateGeosphereObject(
	SE_G::DeferredRenderer* renderSystem)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject>();
	auto tc = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	eastl::shared_ptr<SE_G::Mesh> meshPtr;
	AssetPath meshPath = AssetPath(L"Geosphere");
	meshPath.m_params.asMesh.param1 = 1; // shapeData->NumSubdivisions;
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	meshPtr = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	meshPtr->m_meshPath = meshRes->m_meshPath;

	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	/*auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc->GetDevice(),
		AssetPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine), 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
	mc->SetTexture(texture);*/

	AssetPath texPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	mc->SetTexture(texture);

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
	if (j["components"].contains("Render")) {
		rc->FromJson(j["components"]["Render"]);
	}

	auto shapeData = eastl::make_shared<GeosphereShapeData>(j["m_shapeData"].get<GeosphereShapeData>());

	eastl::shared_ptr<SE_G::Mesh> meshPtr;
	AssetPath meshPath = AssetPath(L"Geosphere");
	meshPath.m_params.asMesh.param1 = shapeData->NumSubdivisions;
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	meshPtr = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	meshPtr->m_meshPath = meshRes->m_meshPath;

	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	AssetPath texPath(L"");
	if (j["components"]["Mesh"].contains("Texture"))
	{
		texPath.FromJson(j["components"]["Mesh"]["Texture"]);
	}
	else {
		texPath = AssetPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine);
	}

	ResourceHandle texHandle = rm.LoadByPath(texPath);
	if (texHandle.guid == 0) {
		// Error
		auto ap = AssetPath(
			SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
			AssetPath::AssetSource::Engine);
		texHandle = rm.LoadByPath(ap);
	}
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	mc->SetTexture(texture);

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateCylinderObject(
	SE_G::DeferredRenderer* renderSystem)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject>();
	auto tc = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);

	eastl::shared_ptr<SE_G::Mesh> meshPtr;
	AssetPath meshPath = AssetPath(L"Cylinder");
	meshPath.m_params.asMesh.param1 = 10; // shapeData->SliceCount;
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	meshPtr = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	meshPtr->m_meshPath = meshRes->m_meshPath;

	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	AssetPath texPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	mc->SetTexture(texture);

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateCylinderObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	eastl::unique_ptr<GameObject> obj = eastl::make_unique<GameObject>();

	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	obj->m_name = "Cylinder";

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = obj->AddComponent<TransformComponent>(device);
	if (j["components"].contains("Transform")) {
		tc->FromJson(j["components"]["Transform"]);
	}

	// RenderComponent and technique
	auto rc = obj->AddComponent<RenderComponent>(obj->m_UUID, renderSystem);
	if (j["components"].contains("Render")) {
		rc->FromJson(j["components"]["Render"]);
	}

	auto shapeData = eastl::make_shared<CylinderShapeData>(j["m_shapeData"].get<CylinderShapeData>());

	eastl::shared_ptr<SE_G::Mesh> meshPtr;
	AssetPath meshPath = AssetPath(L"Cylinder");
	meshPath.m_params.asMesh.param1 = shapeData->SliceCount;
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	meshPtr = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	meshPtr->m_meshPath = meshRes->m_meshPath;

	auto mc = obj->AddComponent<MeshComponent>(rc.get(), tc.get(), obj->m_UUID, meshPtr);

	AssetPath texPath(L"");
	if (j["components"]["Mesh"].contains("Texture"))
	{
		texPath.FromJson(j["components"]["Mesh"]["Texture"]);
	}
	else {
		texPath = AssetPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
	}

	ResourceHandle texHandle = rm.LoadByPath(texPath);
	if (texHandle.guid == 0) {
		// Error
		auto ap = AssetPath(
			SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
			AssetPath::AssetSource::Engine);
		texHandle = ResourceManagerFacade::Instance().LoadByPath(ap);
	}
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	mc->SetTexture(texture);

	return obj;
}

eastl::unique_ptr<SkyBox> GameObjectFactory::CreateSkyBox(
	SE_G::DeferredRenderer* renderSystem,
	AssetPath texturePath,
	SE_G::SkyBoxData initData)
{
	auto obj = eastl::make_unique<SkyBox>(renderSystem, texturePath, initData);
	return obj;
}

eastl::unique_ptr<SkyBox> GameObjectFactory::CreateSkyBox(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = eastl::make_unique<SkyBox>(renderSystem, j);
	return obj;
}

eastl::unique_ptr<AmbientLight> GameObjectFactory::CreateAmbientLightObject(
	SE_G::DeferredRenderer* renderSystem,
	SE_G::AmbientLightData initData)
{
	auto obj = eastl::make_unique<AmbientLight>(renderSystem, initData);
	return obj;
}

eastl::unique_ptr<AmbientLight> GameObjectFactory::CreateAmbientLightObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = eastl::make_unique<AmbientLight>(renderSystem, j);
	return obj;
}

eastl::unique_ptr<DirectionalLight> GameObjectFactory::CreateDirectionalLightObject(
	SE_G::DeferredRenderer* renderSystem,
	SE_G::DirectionalLightData initData)
{
	auto obj = eastl::make_unique<DirectionalLight>(renderSystem, initData, true);
	return obj;
}

eastl::unique_ptr<DirectionalLight> GameObjectFactory::CreateDirectionalLightObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = eastl::make_unique<DirectionalLight>(renderSystem, j);
	return obj;
}

eastl::unique_ptr<PointLight> GameObjectFactory::CreatePointLightObject(
	SE_G::DeferredRenderer* renderSystem,
	SE_G::PointLightData initData)
{
	auto obj = eastl::make_unique<PointLight>(renderSystem, initData);
	return obj;
}

eastl::unique_ptr<PointLight> GameObjectFactory::CreatePointLightObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = eastl::make_unique<PointLight>(renderSystem, j);
	return obj;
}

eastl::unique_ptr<SpotLight> GameObjectFactory::CreateSpotLightObject(
	SE_G::DeferredRenderer* renderSystem,
	SE_G::SpotLightData initData)
{
	auto obj = eastl::make_unique<SpotLight>(renderSystem, initData);
	return obj;
}

eastl::unique_ptr<SpotLight> GameObjectFactory::CreateSpotLightObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = eastl::make_unique<SpotLight>(renderSystem, j);
	return obj;
}
