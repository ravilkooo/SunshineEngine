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
class PointLight;

class GameObjectFactory
{
public:
	eastl::unique_ptr<GameObject> CreateDefaultCubeObject(ID3D11Device* device);

	eastl::unique_ptr<GameObject> CreateDefaultSphereObject(ID3D11Device* device);

	eastl::unique_ptr<GameObject> CreateFinalPassQuad(ID3D11Device* device);

	eastl::unique_ptr<AmbientLight> CreateAmbientLightObject(
		ID3D11Device* device,
		eastl::shared_ptr<Camera> camera,
		AmbientLightData initData = { DXSM::Vector4::One * 0.1 });

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
