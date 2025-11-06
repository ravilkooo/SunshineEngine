#include "GameObjectFactory.h"
#include "GameObject.h"
#include "Component/RenderComponent.h"
#include "Component/TransformComponent.h"
#include "Graphics/Lighting/AmbientLight.h"
#include "Graphics/Lighting/PointLight.h"
#include "Graphics/Lighting/DirectionalLight.h"
#include "Graphics/Lighting/SkyBox.h"

#include "Graphics/Renderer/Technique/GPassTechnique.h"
#include "Graphics/Renderer/Technique/LightTechnique.h"
#include "Graphics/Renderer/Technique/AmbientLightTechnique.h"
#include "Graphics/Renderer/Technique/DirectionalLightTechnique.h"
#include "Graphics/Renderer/Technique/PointLightTechnique.h"
#include "Graphics/Renderer/Technique/SkyBoxTechnique.h"
#include "Graphics/Renderer/Technique/IconTechnique.h"


eastl::unique_ptr<GameObject> GameObjectFactory::CreateDefaultBoxObject(
	ID3D11Device* device,
	float width, float height, float length)
{
    auto obj = eastl::make_unique<GameObject>();
    obj->AddComponent<TransformComponent>(device);
    auto rc = obj->AddComponent<RenderComponent>();

	SE_G::GPassTechnique* gBufferTech = new SE_G::GPassTechnique(device, "GPass", obj->m_UUID);
	gBufferTech->mesh = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(device, width, height, length);

	gBufferTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/GPass/GPassShaderVS.hlsl"));

	gBufferTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/GPass/GPassTextureShaderPS.hlsl"));

	//gBufferTech->texture = eastl::make_shared<SE_G::Bind::Texture>(device, SE_Color(10, 250, 243));
	gBufferTech->texture = eastl::make_shared<SE_G::Bind::Texture>(
		device,
		MakeEngineAssetPath_Wstring(L"DefaultTexture.dds"),
		0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER
	);

	gBufferTech->textureSampler = eastl::make_shared<SE_G::Bind::Sampler>(
		device,
		CD3D11_SAMPLER_DESC(CD3D11_DEFAULT{}),
		0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER
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

	SE_G::GPassTechnique* gBufferTech = new SE_G::GPassTechnique(device, "GPass", obj->m_UUID);
	gBufferTech->mesh = SE_G::Mesh::CreateSphereMesh(device, radius);

	gBufferTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/GPass/GPassShaderVS.hlsl"));

	gBufferTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/GPass/GPassTextureShaderPS.hlsl"));

	//gBufferTech->texture = eastl::make_shared<SE_G::Bind::Texture>(device, SE_Color(10, 250, 243));
	gBufferTech->texture = eastl::make_shared<SE_G::Bind::Texture>(
		device,
		MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"),
		0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER
	);

	gBufferTech->textureSampler = eastl::make_shared<SE_G::Bind::Sampler>(
		device,
		CD3D11_SAMPLER_DESC(CD3D11_DEFAULT{}),
		0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER
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

	SE_G::RenderTechnique* gBufferTech = new SE_G::RenderTechnique(device, "FinalPass");
	gBufferTech->mesh = eastl::make_shared<SE_G::Mesh>(device, "ScreenAlignedQuad");

	gBufferTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/FinalPass/FinalPassVS.hlsl"));

	gBufferTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/FinalPass/FinalPassPS.hlsl"));

	auto p = eastl::make_pair(eastl::string("FinalPass"), gBufferTech);
	rc->techniques.insert(eastl::move(p));

	return obj;
}

eastl::unique_ptr<SE_G::SkyBox> GameObjectFactory::CreateSkyBox(
	ID3D11Device* device,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::SkyBoxData initData, eastl::wstring texturePath)
{
	auto obj = eastl::make_unique<SE_G::SkyBox>(initData);
	obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>();

	// LightPass - LightTechnique
	SE_G::SkyBoxTechnique* lightTech = new SE_G::SkyBoxTechnique(device, "LightPass", camera, obj->m_lightData);

	float toHalfDiag = 1.15; //   2 / sqrt(3) =1,15470053838
	// Add mesh for Ambient
	lightTech->mesh = SE_G::Mesh::CreateUnwrappedBoxMesh(device,
		camera->GetFarZ() * toHalfDiag,
		camera->GetFarZ() * toHalfDiag,
		camera->GetFarZ() * toHalfDiag);

	if (texturePath.empty() || texturePath == L"Default") {
		lightTech->texture = eastl::make_shared<SE_G::Bind::Texture>(
			device,
			MakeEngineAssetPath_Wchar(L"DefaultSkybox.dds"),
			4u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER
		);
	}
	else
	{
		lightTech->texture = eastl::make_shared<SE_G::Bind::Texture>(
			device,
			texturePath,
			4u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER
		);
	}

	lightTech->textureSampler = eastl::make_shared<SE_G::Bind::Sampler>(
		device,
		CD3D11_SAMPLER_DESC(CD3D11_DEFAULT{}),
		1u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER
	);

	lightTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/SkyBoxVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/SkyBoxPShader.hlsl"));

	auto p = eastl::make_pair(eastl::string("LightPass"), lightTech);
	rc->techniques.insert(eastl::move(p));

	// IconPass
	SE_G::IconTechnique* iconTech = new SE_G::IconTechnique(device, "IconPass",
		{ 0u, 0u, 1u, 1u, obj->m_UUID.GetHilo() });
	rc->techniques.insert(eastl::move(eastl::make_pair(eastl::string("IconPass"), iconTech)));

	return obj;
}

eastl::unique_ptr<SE_G::AmbientLight> GameObjectFactory::CreateAmbientLightObject(
	ID3D11Device* device,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::AmbientLightData initData)
{
	auto obj = eastl::make_unique<SE_G::AmbientLight>(initData);
	obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>();

	// LightPass - LightTechnique
	SE_G::AmbientLightTechnique* lightTech = new SE_G::AmbientLightTechnique(device, "LightPass", camera, obj->m_lightData);

	// Add mesh for Ambient
	lightTech->mesh = SE_G::Mesh::CreateScreenAlignedQuad(device);

	lightTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/AmbientLightVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/AmbientLightPShader.hlsl"));

	auto p = eastl::make_pair(eastl::string("LightPass"), lightTech);
	rc->techniques.insert(eastl::move(p));

	// IconPass
	SE_G::IconTechnique* iconTech = new SE_G::IconTechnique(device, "IconPass",
		{ 1u, 0u, 1u, 1u, obj->m_UUID.GetHilo() });
	rc->techniques.insert(eastl::move(eastl::make_pair(eastl::string("IconPass"), iconTech)));

	return obj;
}

eastl::unique_ptr<SE_G::DirectionalLight> GameObjectFactory::CreateDirectionalLightObject(
	ID3D11Device* device,
	eastl::shared_ptr<SE_G::Camera> camera, SE_G::DirectionalLightData initData)
{
	auto obj = eastl::make_unique<SE_G::DirectionalLight>(initData);
	obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>();

	// LightPass - LightTechnique
	SE_G::DirectionalLightTechnique* lightTech =
		new SE_G::DirectionalLightTechnique(device, "LightPass", camera, obj->m_lightData);

	// Add mesh for Ambient
	lightTech->mesh = SE_G::Mesh::CreateScreenAlignedQuad(device);

	lightTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/DirectionalLightVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/DirectionalLightPShader.hlsl"));

	auto p = eastl::make_pair(eastl::string("LightPass"), lightTech);
	rc->techniques.insert(eastl::move(p));

	// IconPass
	SE_G::IconTechnique* iconTech = new SE_G::IconTechnique(device, "IconPass",
		{ 2u, 0u, 1u, 1u, obj->m_UUID.GetHilo() });
	rc->techniques.insert(eastl::move(eastl::make_pair(eastl::string("IconPass"), iconTech)));

	return obj;
}

eastl::unique_ptr<SE_G::PointLight> GameObjectFactory::CreatePointLightObject(
	ID3D11Device* device,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::PointLightData initData)
{
	auto obj = eastl::make_unique<SE_G::PointLight>(initData);
	auto tc = obj->AddComponent<TransformComponent>(device);
	tc->m_position = obj->m_lightData->Position;
	auto rc = obj->AddComponent<RenderComponent>();

	// LightPass - LightTechnique
	SE_G::PointLightTechnique* lightTech =
		new SE_G::PointLightTechnique(device, "LightPass", camera, obj->m_lightData);

	lightTech->m_assignedTransform = tc;

	lightTech->mesh = SE_G::Mesh::CreateGeosphereMesh(device, obj->m_lightData->Range, 1);

	lightTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/PointLightVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/PointLightPShader.hlsl"));

	auto p = eastl::make_pair(eastl::string("LightPass"), lightTech);
	rc->techniques.insert(eastl::move(p));

	// IconPass
	SE_G::IconTechnique* iconTech = new SE_G::IconTechnique(device, "IconPass",
		{ 3u, 0u, 1u, 1u, obj->m_UUID.GetHilo() });
	rc->techniques.insert(eastl::move(eastl::make_pair(eastl::string("IconPass"), iconTech)));

	return obj;
}

