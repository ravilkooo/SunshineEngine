#pragma once
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <Utils/StringUtils.h>
#include <wrl.h>
#include <d3d11.h>
#include <Graphics/Utils/Camera.h>
#include "Graphics/Lighting/LightData.h"

class GameObject;

namespace SE_G {
	class AmbientLight;
	class DirectionalLight;
	class PointLight;
	class SkyBox;
}

class GameObjectFactory
{
public:
	static eastl::unique_ptr<GameObject> CreateDefaultBoxObject(
		ID3D11Device* device,
		float width = 1.0f, float height = 1.0f, float length = 1.0f);

	static eastl::unique_ptr<GameObject> CreateDefaultSphereObject(ID3D11Device* device, float radius = 1.0f);

	static eastl::unique_ptr<GameObject> CreateFinalPassQuad(ID3D11Device* device);

	static eastl::unique_ptr<SE_G::SkyBox> CreateSkyBox(
		ID3D11Device* device,
		eastl::shared_ptr<SE_G::Camera> camera,
		SE_G::SkyBoxData initData = { DXSM::Vector3::One, 0.0f },
		eastl::wstring texturePath = eastl::wstring(L"Default")
);

	static eastl::unique_ptr<SE_G::AmbientLight> CreateAmbientLightObject(
		ID3D11Device* device,
		eastl::shared_ptr<SE_G::Camera> camera,
		SE_G::AmbientLightData initData = { DXSM::Vector3::One * 0.1f, 1.0f });

	static eastl::unique_ptr<SE_G::DirectionalLight> CreateDirectionalLightObject(
		ID3D11Device* device,
		eastl::shared_ptr<SE_G::Camera> camera,
		SE_G::DirectionalLightData initData = {
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3::Zero, 0,
			DXSM::Vector3(1, -1, 1), 0
		});

	static eastl::unique_ptr<SE_G::PointLight> CreatePointLightObject(
		ID3D11Device* device,
		eastl::shared_ptr<SE_G::Camera> camera,
		SE_G::PointLightData initData = {
			DXSM::Vector3::One, 1.0f,
			DXSM::Vector3::One, 1.0f,
			DXSM::Vector3::Zero, 20,
			DXSM::Vector3::One, 0
		});
};
