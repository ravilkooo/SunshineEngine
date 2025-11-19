#pragma once
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <Utils/StringUtils.h>
#include <wrl.h>
#include <d3d11.h>
#include <Graphics/Utils/Camera.h>
#include <Graphics/Lighting/LightData.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class GameObject;

namespace SE_G {
	class DeferredRenderer;
}

class AmbientLight;
class DirectionalLight;
class PointLight;
class SkyBox;

class GameObjectFactory
{
public:
	static eastl::unique_ptr<GameObject> CreateBoxObject(
		SE_G::DeferredRenderer* renderSystem,
		float width, float height, float length);

	static eastl::unique_ptr<GameObject> CreateBoxObject(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);

	static eastl::unique_ptr<GameObject> CreateSphereObject(
		SE_G::DeferredRenderer* renderSystem, float radius);

	static eastl::unique_ptr<GameObject> CreateSphereObject(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);

	static eastl::unique_ptr<GameObject> CreateGeosphereObject(
		SE_G::DeferredRenderer* renderSystem, float radius);

	static eastl::unique_ptr<GameObject> CreateGeosphereObject(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);

	static eastl::unique_ptr<SkyBox> CreateSkyBox(
		SE_G::DeferredRenderer* m_renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		eastl::wstring texturePath = eastl::wstring(L"Default"),
		SE_G::SkyBoxData initData = { DXSM::Vector3::One, 0.0f });

	static eastl::unique_ptr<SkyBox> CreateSkyBox(
		SE_G::DeferredRenderer* renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		const json& j);

	static eastl::unique_ptr<AmbientLight> CreateAmbientLightObject(
		SE_G::DeferredRenderer* m_renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		SE_G::AmbientLightData initData = { DXSM::Vector3::One * 0.1f, 1.0f });

	static eastl::unique_ptr<AmbientLight> CreateAmbientLightObject(
		SE_G::DeferredRenderer* renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		const json& j);

	static eastl::unique_ptr<DirectionalLight> CreateDirectionalLightObject(
		SE_G::DeferredRenderer* m_renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		SE_G::DirectionalLightData initData = {
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3::Zero, 0,
			DXSM::Vector3(1, -1, 1), 0
		});

	static eastl::unique_ptr<DirectionalLight> CreateDirectionalLightObject(
		SE_G::DeferredRenderer* renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		const json& j);

	static eastl::unique_ptr<PointLight> CreatePointLightObject(
		SE_G::DeferredRenderer* m_renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		SE_G::PointLightData initData = {
			DXSM::Vector3::One, 1.0f,
			DXSM::Vector3::One, 1.0f,
			DXSM::Vector3::Zero, 20,
			DXSM::Vector3::One, 0
		});

	static eastl::unique_ptr<PointLight> CreatePointLightObject(
		SE_G::DeferredRenderer* renderSystem,
		eastl::shared_ptr<SE_G::Camera> camera,
		const json& j);
};
