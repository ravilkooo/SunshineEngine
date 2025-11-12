#include <EditorObjectFactory.h>
#include <GameObject.h>
#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>

#include <Graphics/Renderer/DeferredRenderer.h>

#include <Graphics/Lighting/AmbientLight.h>
#include <Graphics/Lighting/PointLight.h>
#include <Graphics/Lighting/DirectionalLight.h>
#include <Graphics/Lighting/SkyBox.h>

#include <Graphics/Renderer/Technique/GPassTechnique.h>
#include <Graphics/Renderer/Technique/LightTechnique.h>
#include <Graphics/Renderer/Technique/AmbientLightTechnique.h>
#include <Graphics/Renderer/Technique/DirectionalLightTechnique.h>
#include <Graphics/Renderer/Technique/PointLightTechnique.h>
#include <Graphics/Renderer/Technique/SkyBoxTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>


eastl::unique_ptr<GameObject_Info> EditorObjectFactory::CreateDefaultBoxObject(
	SE_G::DeferredRenderer* renderSystem,
	float width, float height, float length)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject_Info>();
	obj->m_group = GameObjectGroup::Shapes;
	obj->m_name = "Box";

	// TransformComponent
	auto tr_info = obj->AddComponent<TransformComponent_Info>();
	tr_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);

	// RenderComponent and Passes
	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(device, tr_info->m_assignedComponent.get(), "GPass", obj->m_UUID);
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

	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);
	rc_info->AddTechnique(eastl::move(gBufferTech));

	return obj;
}

eastl::unique_ptr<GameObject_Info> EditorObjectFactory::CreateDefaultSphereObject(SE_G::DeferredRenderer* renderSystem, float radius)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject_Info>();
	obj->m_group = GameObjectGroup::Shapes;
	obj->m_name = "Sphere";

	// TransformComponent
	auto tr_info = obj->AddComponent<TransformComponent_Info>();
	tr_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);

	// RenderComponent and Passes
	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(device, tr_info->m_assignedComponent.get(), "GPass", obj->m_UUID);
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

	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);
	rc_info->AddTechnique(eastl::move(gBufferTech));

	return obj;
}

eastl::unique_ptr<GameObject_Info> EditorObjectFactory::CreateFinalPassQuad(SE_G::DeferredRenderer* renderSystem)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<GameObject_Info>();
	obj->m_group = GameObjectGroup::Other;
	obj->m_name = "FinalPass";

	// TransformComponent
	auto tr_info = obj->AddComponent<TransformComponent_Info>();
	tr_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);

	// RenderComponent and Passes
	auto gBufferTech = eastl::make_unique<SE_G::RenderTechnique>(device, tr_info->m_assignedComponent.get(), "FinalPass");
	gBufferTech->mesh = eastl::make_shared<SE_G::Mesh>(device, "ScreenAlignedQuad");

	gBufferTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/FinalPass/FinalPassVS.hlsl"));

	gBufferTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/FinalPass/FinalPassPS.hlsl"));

	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);
	rc_info->AddTechnique(eastl::move(gBufferTech));

	return obj;
}

eastl::unique_ptr<SkyBox_Info> EditorObjectFactory::CreateSkyBox(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::SkyBoxData initData, eastl::wstring texturePath)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<SkyBox_Info>();

	// TransformComponent
	auto tr_info = obj->AddComponent<TransformComponent_Info>();
	tr_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);

	// RenderComponent and Passes

	// LightPass - LightTechnique
	auto lightTech = eastl::make_unique<SE_G::SkyBoxTechnique>(device, tr_info->m_assignedComponent.get(), "LightPass", camera, obj->m_lightData);

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

	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);
	rc_info->AddTechnique(eastl::move(lightTech));

	// IconPass
	auto iconTech = eastl::make_unique<SE_G::IconTechnique>(device, tr_info->m_assignedComponent.get(), eastl::string("IconPass"),
		SE_G::IconData{ 0u, 0u, 1u, 1u, obj->m_UUID.GetHilo() });

	rc_info->AddTechnique(eastl::move(iconTech));

	return obj;
}

eastl::unique_ptr<AmbientLight_Info> EditorObjectFactory::CreateAmbientLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::AmbientLightData initData)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<AmbientLight_Info>(initData);

	// TransformComponent
	auto tr_info = obj->AddComponent<TransformComponent_Info>();
	tr_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);

	// RenderComponent and Passes

	// LightPass - LightTechnique
	auto lightTech = eastl::make_unique<SE_G::AmbientLightTechnique>(device, tr_info->m_assignedComponent.get(), "LightPass", camera, obj->m_lightData);

	// Add mesh for Ambient
	lightTech->mesh = SE_G::Mesh::CreateScreenAlignedQuad(device);

	lightTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/AmbientLightVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/AmbientLightPShader.hlsl"));

	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);
	rc_info->AddTechnique(eastl::move(lightTech));

	// IconPass
	auto iconTech = eastl::make_unique<SE_G::IconTechnique>(device, tr_info->m_assignedComponent.get(), eastl::string("IconPass"),
		SE_G::IconData{ 1u, 0u, 1u, 1u, obj->m_UUID.GetHilo() });

	rc_info->AddTechnique(eastl::move(iconTech));

	return obj;
}

eastl::unique_ptr<DirectionalLight_Info> EditorObjectFactory::CreateDirectionalLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera, SE_G::DirectionalLightData initData)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<DirectionalLight_Info>(initData);

	// TransformComponent
	auto tr_info = obj->AddComponent<TransformComponent_Info>();
	tr_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);

	// RenderComponent and Passes

	// LightPass - LightTechnique
	auto lightTech =
		eastl::make_unique<SE_G::DirectionalLightTechnique>(device, tr_info->m_assignedComponent.get(), "LightPass", camera, obj->m_lightData);

	// Add mesh for Ambient
	lightTech->mesh = SE_G::Mesh::CreateScreenAlignedQuad(device);

	lightTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/DirectionalLightVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/DirectionalLightPShader.hlsl"));

	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);
	rc_info->AddTechnique(eastl::move(lightTech));

	// IconPass
	auto iconTech = eastl::make_unique<SE_G::IconTechnique>(device, tr_info->m_assignedComponent.get(), eastl::string("IconPass"),
		SE_G::IconData{ 2u, 0u, 1u, 1u, obj->m_UUID.GetHilo() });

	rc_info->AddTechnique(eastl::move(iconTech));

	return obj;
}

eastl::unique_ptr<PointLight_Info> EditorObjectFactory::CreatePointLightObject(
	SE_G::DeferredRenderer* renderSystem,
	eastl::shared_ptr<SE_G::Camera> camera,
	SE_G::PointLightData initData)
{
	auto device = renderSystem->GetDevice();

	auto obj = eastl::make_unique<PointLight_Info>(initData);

	// TransformComponent
	auto tr_info = obj->AddComponent<TransformComponent_Info>();
	tr_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);
	tr_info->m_assignedComponent->m_position = initData.Position;

	// RenderComponent and Passes

	// LightPass - LightTechnique
	auto lightTech =
		eastl::make_unique<SE_G::PointLightTechnique>(device, tr_info->m_assignedComponent.get(), "LightPass", camera, obj->m_lightData);

	lightTech->mesh = SE_G::Mesh::CreateGeosphereMesh(device, obj->m_lightData->Range, 1);

	lightTech->vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/PointLightVShader.hlsl"));

	lightTech->pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/PointLightPShader.hlsl"));

	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);
	rc_info->AddTechnique(eastl::move(lightTech));

	// IconPass
	auto iconTech = eastl::make_unique<SE_G::IconTechnique>(device, tr_info->m_assignedComponent.get(), eastl::string("IconPass"),
		SE_G::IconData{ 3u, 0u, 1u, 1u, obj->m_UUID.GetHilo() });

	rc_info->AddTechnique(eastl::move(iconTech));

	return obj;
}

