#pragma once
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <Utils/StringUtils.h>
#include <wrl.h>
#include <d3d11.h>
#include <GraphicsUtils/Camera.h>
#include "Graphics/Lighting/LightData.h"

class GameObject;
class AmbientLight;
class DirectionalLight;
class PointLight;

class GameObjectFactory
{
public:
	eastl::unique_ptr<GameObject> CreateDefaultBoxObject(
		ID3D11Device* device,
		float width = 1.0f, float height = 1.0f, float length = 1.0f);

	eastl::unique_ptr<GameObject> CreateDefaultSphereObject(ID3D11Device* device, float radius = 1.0f);

	eastl::unique_ptr<GameObject> CreateFinalPassQuad(ID3D11Device* device);

	eastl::unique_ptr<AmbientLight> CreateAmbientLightObject(
		ID3D11Device* device,
		eastl::shared_ptr<Camera> camera,
		AmbientLightData initData = { DXSM::Vector4::One * 0.1 });

	eastl::unique_ptr<DirectionalLight> CreateDirectionalLightObject(
		ID3D11Device* device,
		eastl::shared_ptr<Camera> camera,
		DirectionalLightData initData = {
			DXSM::Vector4(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f, 1.0f),
			DXSM::Vector4(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f, 1.0f),
			DXSM::Vector3::Zero, 0,
			DXSM::Vector3(0, -1, 0), 0
		});

	eastl::unique_ptr<PointLight> CreatePointLightObject(
		ID3D11Device* device,
		eastl::shared_ptr<Camera> camera,
		PointLightData initData = {
			DXSM::Vector4::One,
			DXSM::Vector4::One,
			DXSM::Vector3::Zero, 20,
			DXSM::Vector3::One, 0
		});
};
