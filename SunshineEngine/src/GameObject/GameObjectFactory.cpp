#include <GameObject/GameObjectFactory.h>
#include <GameObject/GameObject.h>
#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>
#include <GameObject/Lighting/AmbientLight.h>
#include <GameObject/Lighting/PointLight.h>
#include <GameObject/Lighting/DirectionalLight.h>
#include <GameObject/Lighting/SkyBox.h>

#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/Technique/GPassTechnique.h>
#include <Graphics/Renderer/Technique/LightTechnique.h>
#include <Graphics/Renderer/Technique/AmbientLightTechnique.h>
#include <Graphics/Renderer/Technique/DirectionalLightTechnique.h>
#include <Graphics/Renderer/Technique/PointLightTechnique.h>
#include <Graphics/Renderer/Technique/SkyBoxTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>


eastl::unique_ptr<GameObject> GameObjectFactory::CreateDefaultBoxObject(
	SE_G::DeferredRenderer* renderSystem,
	float width, float height, float length)
{
	auto device = renderSystem->GetDevice();

    auto obj = eastl::make_unique<GameObject>();
	auto tr = obj->AddComponent<TransformComponent>();
    auto rc = obj->AddComponent<RenderComponent>(renderSystem);

	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(renderSystem, tr.get(), "GPass", obj->m_UUID);
	gBufferTech->m_mesh = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(device, width, height, length);
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultTexture.dds"));

	rc->AddTechnique(eastl::move(gBufferTech));

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateDefaultSphereObject(SE_G::DeferredRenderer* renderSystem, float radius)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject>();
	auto tr = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(renderSystem);

	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(renderSystem, tr.get(), "GPass", obj->m_UUID);
	gBufferTech->m_mesh = SE_G::Mesh::CreateSphereMesh(device, radius);
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"));

	rc->AddTechnique(eastl::move(gBufferTech));

	return obj;
}

eastl::unique_ptr<SkyBox> GameObjectFactory::CreateSkyBox(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	eastl::wstring texturePath,
	SE_G::SkyBoxData initData)
{
	auto obj = eastl::make_unique<SkyBox>(renderSystem, camera, texturePath, initData);
	return eastl::move(obj);
}

eastl::unique_ptr<AmbientLight> GameObjectFactory::CreateAmbientLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::AmbientLightData initData)
{
	auto obj = eastl::make_unique<AmbientLight>(renderSystem, camera, initData);
	return eastl::move(obj);
}

eastl::unique_ptr<DirectionalLight> GameObjectFactory::CreateDirectionalLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::DirectionalLightData initData)
{
	auto obj = eastl::make_unique<DirectionalLight>(renderSystem, camera, initData);
	return eastl::move(obj);
}

eastl::unique_ptr<PointLight> GameObjectFactory::CreatePointLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::PointLightData initData)
{
	auto obj = eastl::make_unique<PointLight>(renderSystem, camera, initData);
	return eastl::move(obj);
}

