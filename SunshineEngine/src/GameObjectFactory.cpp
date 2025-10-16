#include "GameObjectFactory.h"
#include "GameObject.h"
#include "Component/RenderComponent.h"
#include "Component/TransformComponent.h"
#include "Graphics/Lighting/AmbientLight.h"
#include "Graphics/Lighting/PointLight.h"
#include "Graphics/Lighting/DirectionalLight.h"

#include "Graphics/LightTechnique.h"
#include "Graphics/AmbientLightTechnique.h"
#include "Graphics/DirectionalLightTechnique.h"
#include "Graphics/PointLightTechnique.h"

eastl::unique_ptr<GameObject> GameObjectFactory::CreateDefaultBoxObject(
	ID3D11Device* device,
	float width, float height, float length)
{
    auto obj = eastl::make_unique<GameObject>();
    obj->AddComponent<TransformComponent>(device);
    auto rc = obj->AddComponent<RenderComponent>();

	RenderTechnique* gBufferTech = new RenderTechnique(device, "GPass");
	gBufferTech->mesh = Mesh::CreateUnwrappedBoxMesh_repeat(device, width, height, length);

	gBufferTech->vertexShader = eastl::make_shared<Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/GPass/GPassShaderVS.hlsl"));

	gBufferTech->pixelShader = eastl::make_shared<Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/GPass/GPassTextureShaderPS.hlsl"));

	//gBufferTech->texture = eastl::make_shared<Bind::Texture>(device, SE_Color(10, 250, 243));
	gBufferTech->texture = eastl::make_shared<Bind::Texture>(
		device,
		MakeEngineAssetPath_Wstring(L"DefaultTexture.dds"),
		0u,
		Bind::PipelineStage::PIXEL_SHADER
	);

	gBufferTech->textureSampler = eastl::make_shared<Bind::Sampler>(
		device,
		CD3D11_SAMPLER_DESC(CD3D11_DEFAULT{}),
		0u,
		Bind::PipelineStage::PIXEL_SHADER
	);

	auto p = eastl::make_pair(eastl::string("GPass"), gBufferTech);
	rc->techniques.insert(eastl::move(p));

    return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateDefaultSphereObject(ID3D11Device* device, float radius)
{
	auto obj = eastl::make_unique<GameObject>();
	obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>();

	RenderTechnique* gBufferTech = new RenderTechnique(device, "GPass");
	gBufferTech->mesh = Mesh::CreateSphereMesh(device, radius);

	gBufferTech->vertexShader = eastl::make_shared<Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/GPass/GPassShaderVS.hlsl"));

	gBufferTech->pixelShader = eastl::make_shared<Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/GPass/GPassTextureShaderPS.hlsl"));

	//gBufferTech->texture = eastl::make_shared<Bind::Texture>(device, SE_Color(10, 250, 243));
	gBufferTech->texture = eastl::make_shared<Bind::Texture>(
		device,
		MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"),
		0u,
		Bind::PipelineStage::PIXEL_SHADER
	);

	gBufferTech->textureSampler = eastl::make_shared<Bind::Sampler>(
		device,
		CD3D11_SAMPLER_DESC(CD3D11_DEFAULT{}),
		0u,
		Bind::PipelineStage::PIXEL_SHADER
	);

	auto p = eastl::make_pair(eastl::string("GPass"), gBufferTech);
	rc->techniques.insert(eastl::move(p));

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateFinalPassQuad(ID3D11Device* device)
{
	auto obj = eastl::make_unique<GameObject>();
	obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>();

	RenderTechnique* gBufferTech = new RenderTechnique(device, "FinalPass");
	gBufferTech->mesh = eastl::make_shared<Mesh>(device, "ScreenAlignedQuad");

	gBufferTech->vertexShader = eastl::make_shared<Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/FinalPass/FinalPassVS.hlsl"));

	gBufferTech->pixelShader = eastl::make_shared<Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/FinalPass/FinalPassPS.hlsl"));

	auto p = eastl::make_pair(eastl::string("FinalPass"), gBufferTech);
	rc->techniques.insert(eastl::move(p));

	return obj;
}

eastl::unique_ptr<AmbientLight> GameObjectFactory::CreateAmbientLightObject(
	ID3D11Device* device,
	eastl::shared_ptr<Camera> camera,
	AmbientLightData initData)
{
	auto obj = eastl::make_unique<AmbientLight>(initData);
	obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>();

	// LightPass - LightTechnique
	AmbientLightTechnique* lightTech = new AmbientLightTechnique(device, "LightPass");
	
	lightTech->lightData = obj->ambientLightData;
	lightTech->lightDataBuffer =
		eastl::make_shared<Bind::PixelConstantBuffer<AmbientLightData>>(
			device,
			*(obj->ambientLightData),
			1u
	);

	lightTech->m_camera = camera;

	// Add mesh for Ambient
	lightTech->mesh = Mesh::CreateScreenAlignedQuad(device);

	lightTech->vertexShader = eastl::make_shared<Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/AmbientLightVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/AmbientLightPShader.hlsl"));

	auto p = eastl::make_pair(eastl::string("LightPass"), lightTech);
	rc->techniques.insert(eastl::move(p));

	return obj;
}

eastl::unique_ptr<DirectionalLight> GameObjectFactory::CreateDirectionalLightObject(
	ID3D11Device* device,
	eastl::shared_ptr<Camera> camera, DirectionalLightData initData)
{
	auto obj = eastl::make_unique<DirectionalLight>(initData);
	obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>();

	// LightPass - LightTechnique
	DirectionalLightTechnique* lightTech = new DirectionalLightTechnique(device, "LightPass");

	lightTech->lightData = obj->directionalLightData;
	lightTech->lightDataBuffer =
		eastl::make_shared<Bind::PixelConstantBuffer<DirectionalLightData>>(
			device,
			*(obj->directionalLightData),
			1u
		);

	lightTech->m_camera = camera;

	// Add mesh for Ambient
	lightTech->mesh = Mesh::CreateScreenAlignedQuad(device);

	lightTech->vertexShader = eastl::make_shared<Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/DirectionalLightVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/DirectionalLightPShader.hlsl"));

	auto p = eastl::make_pair(eastl::string("LightPass"), lightTech);
	rc->techniques.insert(eastl::move(p));

	return obj;
}

eastl::unique_ptr<PointLight> GameObjectFactory::CreatePointLightObject(
	ID3D11Device* device,
	eastl::shared_ptr<Camera> camera,
	PointLightData initData)
{
	auto obj = eastl::make_unique<PointLight>(initData);
	auto tc = obj->AddComponent<TransformComponent>(device);
	tc->m_position = obj->pointLightData->Position;
	auto rc = obj->AddComponent<RenderComponent>();

	// LightPass - LightTechnique
	PointLightTechnique* lightTech = new PointLightTechnique(device, "LightPass");
	
	lightTech->lightData = obj->pointLightData;
	lightTech->lightDataBuffer =
		eastl::make_shared<Bind::PixelConstantBuffer<PointLightData>>(
			device,
			*(obj->pointLightData),
			1u
		);

	lightTech->m_camera = camera;

	// Add mesh for pointlight
	lightTech->mesh = Mesh::CreateGeosphereMesh(device, obj->pointLightData->Range, 1);

	lightTech->vertexShader = eastl::make_shared<Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/PointLightVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/PointLightPShader.hlsl"));

	auto p = eastl::make_pair(eastl::string("LightPass"), lightTech);
	rc->techniques.insert(eastl::move(p));

	return obj;
}

