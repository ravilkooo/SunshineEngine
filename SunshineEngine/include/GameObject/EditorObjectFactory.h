#pragma once

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <Utils/StringUtils.h>
#include <wrl.h>
#include <d3d11.h>
#include <Graphics/Utils/Camera.h>
#include <Graphics/Lighting/LightData.h>

#include <GameObject/GameObject.h>
#include <GameObject/Lighting/LightCollection.h>
#include <GameObject/Shapes/ShapeCollection.h>

#include <Utils/AssetPath.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace SE
{
	class ParticleSystem;
}

namespace SE_G {
	class DeferredRenderer;
}

class EditorObjectFactory
{
public:
	static eastl::unique_ptr<GameObject_Info> CreateParticleEmitter(
		SE::ParticleSystem* particleSystem);

	static eastl::unique_ptr<GameObject_Info> CreateParticleEmitter(
		SE::ParticleSystem* particleSystem,
		const json& j);

	static eastl::unique_ptr<GameObject_Info> CreateCustomMesh(
		SE_G::DeferredRenderer* renderSystem,
		AssetPath meshPath);

	static eastl::unique_ptr<GameObject_Info> CreateCustomMesh(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);

	static eastl::unique_ptr<BoxShapeObject_Info> CreateBoxObject(
		SE_G::DeferredRenderer* renderSystem,
		float width, float height, float length);

	static eastl::unique_ptr<BoxShapeObject_Info> CreateBoxObject(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);

	static eastl::unique_ptr<SphereShapeObject_Info> CreateSphereObject(
		SE_G::DeferredRenderer* renderSystem, float radius);

	static eastl::unique_ptr<SphereShapeObject_Info> CreateSphereObject(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);

	static eastl::unique_ptr<GeosphereShapeObject_Info> CreateGeosphereObject(
		SE_G::DeferredRenderer* renderSystem, float radius);

	static eastl::unique_ptr<GeosphereShapeObject_Info> CreateGeosphereObject(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);

	static eastl::unique_ptr<CylinderShapeObject_Info> CreateCylinderObject(
		SE_G::DeferredRenderer* renderSystem, float radius, float height);

	static eastl::unique_ptr<CylinderShapeObject_Info> CreateCylinderObject(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);

	static eastl::unique_ptr<SkyBox_Info> CreateSkyBox(
		SE_G::DeferredRenderer* renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		AssetPath assetPath = AssetPath(eastl::wstring(L"Textures/DefaultSkybox.dds"), AssetPath::AssetSource::Engine),
		SE_G::SkyBoxData initData = { DXSM::Vector3::One, 0.0f }
	);

	static eastl::unique_ptr<AmbientLight_Info> CreateAmbientLightObject(
		SE_G::DeferredRenderer* renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		SE_G::AmbientLightData initData = { DXSM::Vector3::One * 0.1f, 1.0f });

	static eastl::unique_ptr<DirectionalLight_Info> CreateDirectionalLightObject(
		SE_G::DeferredRenderer* renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		SE_G::DirectionalLightData initData = {
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3::Zero, 0,
			DXSM::Vector2(0, -DX::XM_PIDIV4), 0, 0
		});

	static eastl::unique_ptr<PointLight_Info> CreatePointLightObject(
		SE_G::DeferredRenderer* renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		SE_G::PointLightData initData = {
			DXSM::Vector3::One, 1.0f,
			DXSM::Vector3::One, 1.0f,
			DXSM::Vector3::Zero, 20,
			DXSM::Vector3::One, 0
		});

	static eastl::unique_ptr<SpotLight_Info> CreateSpotLightObject(
		SE_G::DeferredRenderer* renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		SE_G::SpotLightData initData = {
			DXSM::Vector3::One, 1.0f,
			DXSM::Vector3::One, 1.0f,
			DXSM::Vector3::Zero, 20,
			DXSM::Vector2(0, -DX::XM_PIDIV4), 10, 0,
			DXSM::Vector3::One, 0
		});
};
