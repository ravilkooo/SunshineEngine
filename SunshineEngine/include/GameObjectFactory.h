#pragma once
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <Utils/StringUtils.h>
#include <wrl.h>
#include <d3d11.h>
#include <GraphicsUtils/Camera.h>

class GameObject;
class AmbientLight;
class PointLight;

class GameObjectFactory
{
public:
	eastl::unique_ptr<GameObject> CreateDefaultGameObject(ID3D11Device* device);

	eastl::unique_ptr<GameObject> CreateFinalPassQuad(ID3D11Device* device);

	eastl::unique_ptr<AmbientLight> CreateAmbientLightObject(ID3D11Device* device, eastl::shared_ptr<Camera> camera);

	eastl::unique_ptr<PointLight> CreatePointLightObject(ID3D11Device* device, eastl::shared_ptr<Camera> camera);
};
