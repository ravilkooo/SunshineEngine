#include <GameObject/EditorObjectFactory.h>
#include <GameObject/GameObject.h>

#include <GameObject/Lighting/LightCollection.h>

#include <GameObject/Shapes/ShapeCollection.h>

#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>

#include <Graphics/Renderer/DeferredRenderer.h>

#include <Graphics/Renderer/Technique/GPassTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>

eastl::unique_ptr<BoxShapeObject_Info> EditorObjectFactory::CreateDefaultBoxObject(
	SE_G::DeferredRenderer* renderSystem,
	float width, float height, float length)
{
	auto obj = eastl::make_unique<BoxShapeObject_Info>(renderSystem,
		BoxShapeData{ DXSM::Vector3(width, height, length) });
	return eastl::move(obj);
}

eastl::unique_ptr<BoxShapeObject_Info> EditorObjectFactory::CreateDefaultBoxObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = BoxShapeObject_Info::FromJson(renderSystem, j);
	return eastl::move(obj);
}

eastl::unique_ptr<SphereShapeObject_Info> EditorObjectFactory::CreateDefaultSphereObject(
	SE_G::DeferredRenderer* renderSystem, float radius)
{
	auto obj = eastl::make_unique<SphereShapeObject_Info>(renderSystem,
		SphereShapeData{ DXSM::Vector3::One * radius, 10u, 10u });
	return eastl::move(obj);
}

eastl::unique_ptr<SphereShapeObject_Info> EditorObjectFactory::CreateDefaultSphereObject(
	SE_G::DeferredRenderer* renderSystem,
	const json& j)
{
	auto obj = SphereShapeObject_Info::FromJson(renderSystem, j);
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


