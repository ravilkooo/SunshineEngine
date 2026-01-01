#include <GameObject/EditorObjectFactory.h>
#include <GameObject/GameObject.h>

#include <GameObject/Lighting/LightCollection.h>

#include <GameObject/Shapes/ShapeCollection.h>

#include <ParticleSystem/ParticleEmitter.h>
#include <ParticleSystem/ParticleSystem.h>

#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>

#include <Graphics/Renderer/DeferredRenderer.h>

#include <Graphics/Renderer/Technique/GPassTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>


eastl::unique_ptr<SE::ParticleEmitter_Info> EditorObjectFactory::CreateParticleEmitter(
	SE::ParticleSystem* particleSystem)
{
	SE::ParticleData::EmitterPointConstantBuffer emitterDesc;
	SE::ParticleData::SimulateParticlesConstantBuffer simulatorDesc;

	// Bubble Particles
	emitterDesc =
	{
		DXSM::Matrix::Identity,
		{ 15, 0, 0 }, 3.0f,
		{ 1, 1, 1 }, 1.0f,
		{ 1, 1, 1 }, 1.0f,

		8, 1, 0.2, 0.5,
		
		0, DX::XM_2PI, -DX::XM_PI / 10, DX::XM_PI / 10,
		
		100u, { 0, 0, 0 },
	};
	simulatorDesc = {
		{ 0, -5, 0 }, 0
	};

	auto go = eastl::make_unique<SE::ParticleEmitter_Info>(
		particleSystem,
		emitterDesc,
		simulatorDesc);

	AssetPath particleTexPath(L"DefaultTexture.dds");

	auto particleTex = eastl::make_shared<SE_G::Bind::Texture>(particleSystem->m_renderer->GetDevice(), particleTexPath, 0u);

	go->m_particleData->SetTexture(particleTex);
	go->m_particleData->SetEmissionRate(40);

	return go;
}

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

	auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc_info->GetDevice(),
		AssetPath(L"DefaultTexture.dds", AssetPath::AssetSource::Engine), 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
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

	auto mc_info = obj->AddComponent<MeshComponent_Info>();
	mc_info->FromJson(j["components"]["Mesh"],
		device, rc_info.get(),
		tc_info.get(), obj->m_UUID);

	return obj;
}

eastl::unique_ptr<BoxShapeObject_Info> EditorObjectFactory::CreateBoxObject(
	SE_G::DeferredRenderer* renderSystem,
	float width, float height, float length)
{
	auto obj = eastl::make_unique<BoxShapeObject_Info>(renderSystem,
		BoxShapeData{ DXSM::Vector3(width, height, length) });
	return obj;
}

eastl::unique_ptr<BoxShapeObject_Info> EditorObjectFactory::CreateBoxObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = BoxShapeObject_Info::FromJson(renderSystem, j);
	return obj;
}

eastl::unique_ptr<SphereShapeObject_Info> EditorObjectFactory::CreateSphereObject(
	SE_G::DeferredRenderer* renderSystem, float radius)
{
	auto obj = eastl::make_unique<SphereShapeObject_Info>(renderSystem,
		SphereShapeData{ DXSM::Vector3::One * radius, 10u, 10u });
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
	SE_G::DeferredRenderer* renderSystem, float radius)
{
	auto obj = eastl::make_unique<GeosphereShapeObject_Info>(renderSystem,
		GeosphereShapeData{ DXSM::Vector3::One * radius, 2u });
	return obj;
}

eastl::unique_ptr<GeosphereShapeObject_Info> EditorObjectFactory::CreateGeosphereObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = GeosphereShapeObject_Info::FromJson(renderSystem, j);
	return obj;
}

eastl::unique_ptr<SkyBox_Info> EditorObjectFactory::CreateSkyBox(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	AssetPath assetPath,
	SE_G::SkyBoxData initData)
{
	auto obj = eastl::make_unique<SkyBox_Info>(renderSystem, camera, assetPath, initData);
	return obj;
}

eastl::unique_ptr<AmbientLight_Info> EditorObjectFactory::CreateAmbientLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::AmbientLightData initData)
{
	auto obj = eastl::make_unique<AmbientLight_Info>(renderSystem, camera, initData);
	return obj;
}

eastl::unique_ptr<DirectionalLight_Info> EditorObjectFactory::CreateDirectionalLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::DirectionalLightData initData)
{
	auto obj = eastl::make_unique<DirectionalLight_Info>(renderSystem, camera, initData);
	return obj;
}

eastl::unique_ptr<PointLight_Info> EditorObjectFactory::CreatePointLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::PointLightData initData)
{
	auto obj = eastl::make_unique<PointLight_Info>(renderSystem, camera, initData);
	return obj;
}

eastl::unique_ptr<SpotLight_Info> EditorObjectFactory::CreateSpotLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::SpotLightData initData)
{
	auto obj = eastl::make_unique<SpotLight_Info>(renderSystem, camera, initData);
	return obj;
}
