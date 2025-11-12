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

	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(device, tr.get(), "GPass", obj->m_UUID);
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

	rc->AddTechnique(eastl::move(gBufferTech));

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateDefaultSphereObject(SE_G::DeferredRenderer* renderSystem, float radius)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject>();
	auto tr = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(renderSystem);

	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(device, tr.get(), "GPass", obj->m_UUID);
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

	rc->AddTechnique(eastl::move(gBufferTech));

	return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateFinalPassQuad(SE_G::DeferredRenderer* renderSystem)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject>();
	auto tr = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(renderSystem);

	auto gBufferTech = eastl::make_unique<SE_G::RenderTechnique>(device, tr.get(), "FinalPass");
	gBufferTech->mesh = eastl::make_shared<SE_G::Mesh>(device, "ScreenAlignedQuad");

	gBufferTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/FinalPass/FinalPassVS.hlsl"));

	gBufferTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/FinalPass/FinalPassPS.hlsl"));

	rc->AddTechnique(eastl::move(gBufferTech));

	return obj;
}

eastl::unique_ptr<SkyBox> GameObjectFactory::CreateSkyBox(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::SkyBoxData initData, eastl::wstring texturePath)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<SkyBox>(initData);
	auto tr = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(renderSystem);

	// LightPass - LightTechnique
	auto lightTech = eastl::make_unique<SE_G::SkyBoxTechnique>(device, tr.get(), "LightPass", camera, obj->m_lightData);

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

	rc->AddTechnique(eastl::move(lightTech));

	/*
	// IconPass
	SE_G::IconTechnique* iconTech = new SE_G::IconTechnique(device, "IconPass",
		{ 0u, 0u, 1u, 1u, obj->m_UUID.GetHilo() });
	rc->techniques.insert(eastl::move(eastl::make_pair(eastl::string("IconPass"), iconTech)));
	*/

	return obj;
}

eastl::unique_ptr<AmbientLight> GameObjectFactory::CreateAmbientLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::AmbientLightData initData)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<AmbientLight>(initData);
	auto tr = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(renderSystem);

	// LightPass - LightTechnique
	auto lightTech = eastl::make_unique<SE_G::AmbientLightTechnique>(device, tr.get(), "LightPass", camera, obj->m_lightData);

	// Add mesh for Ambient
	lightTech->mesh = SE_G::Mesh::CreateScreenAlignedQuad(device);

	lightTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/AmbientLightVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/AmbientLightPShader.hlsl"));

	rc->AddTechnique(eastl::move(lightTech));

	/*
	// IconPass
	SE_G::IconTechnique* iconTech = new SE_G::IconTechnique(device, "IconPass",
		{ 1u, 0u, 1u, 1u, obj->m_UUID.GetHilo() });
	rc->techniques.insert(eastl::move(eastl::make_pair(eastl::string("IconPass"), iconTech)));
	*/

	return obj;
}

eastl::unique_ptr<DirectionalLight> GameObjectFactory::CreateDirectionalLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera, SE_G::DirectionalLightData initData)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<DirectionalLight>(initData);
	auto tr = obj->AddComponent<TransformComponent>(device);
	auto rc = obj->AddComponent<RenderComponent>(renderSystem);

	// LightPass - LightTechnique
	auto lightTech =
		eastl::make_unique<SE_G::DirectionalLightTechnique>(device, tr.get(), "LightPass", camera, obj->m_lightData);

	// Add mesh for Ambient
	lightTech->mesh = SE_G::Mesh::CreateScreenAlignedQuad(device);

	lightTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/DirectionalLightVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/DirectionalLightPShader.hlsl"));

	rc->AddTechnique(eastl::move(lightTech));

	/*
	// IconPass
	SE_G::IconTechnique* iconTech = new SE_G::IconTechnique(device, "IconPass",
		{ 2u, 0u, 1u, 1u, obj->m_UUID.GetHilo() });
	rc->techniques.insert(eastl::move(eastl::make_pair(eastl::string("IconPass"), iconTech)));
	*/

	return obj;
}

eastl::unique_ptr<PointLight> GameObjectFactory::CreatePointLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::PointLightData initData)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<PointLight>(initData);
	auto tc = obj->AddComponent<TransformComponent>(device);
	tc->m_position = obj->m_lightData->Position;
	auto rc = obj->AddComponent<RenderComponent>(renderSystem);

	// LightPass - LightTechnique
	auto lightTech =
		eastl::make_unique<SE_G::PointLightTechnique>(device, tc.get(), "LightPass", camera, obj->m_lightData);

	lightTech->mesh = SE_G::Mesh::CreateGeosphereMesh(device, obj->m_lightData->Range, 1);

	lightTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/PointLightVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/PointLightPShader.hlsl"));

	rc->AddTechnique(eastl::move(lightTech));

	/*
	// IconPass
	SE_G::IconTechnique* iconTech = new SE_G::IconTechnique(device, "IconPass",
		{ 3u, 0u, 1u, 1u, obj->m_UUID.GetHilo() });
	rc->techniques.insert(eastl::move(eastl::make_pair(eastl::string("IconPass"), iconTech)));
	*/

	return obj;
}

