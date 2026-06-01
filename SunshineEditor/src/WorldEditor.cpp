#include <WorldEditor.h>

#include <d3dcompiler.h>

#include <fstream>   // std::ofstream
#include <EASTL/unique_ptr.h>

#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/MiniViewRenderer.h>

#include <Graphics/Renderer/Pass/GPass.h>
#include <Graphics/Renderer/Pass/LightPass.h>
#include <Graphics/Renderer/Pass/SelectionPass.h>
#include <Graphics/Renderer/Pass/IconPass.h>
#include <Graphics/Renderer/Pass/ColliderPass.h>
#include <Graphics/Renderer/Pass/EmitterDebugPass.h>
#include <Graphics/Renderer/Pass/PerceptionDebugPass.h>

#include <Graphics/Renderer/GBuffer.h>

#include <Graphics/Utils/Camera.h>

#include <Scene.h>

#include <GameObject/GameObject.h>
#include <GameObject/Lighting/LightCollection.h>
#include <GameObject/Shapes/ShapeCollection.h>
#include <GameObject/EditorObjectFactory.h>

#include <Component/PhysicsComponent.h>
#include <Component/LuaComponent.h>
#include <Component/CameraComponent.h>

#include <CameraManager.h>

#include <ParticleSystem/ParticleSystem.h>
// #include <ParticleSystem/ParticleEmitter.h>

#include <SceneHierarchy.h>

#include <ResourceManager/ResourceLoaderFactory.h>

WorldEditor::WorldEditor()
{
	m_timer = GameTimer();
}

WorldEditor::~WorldEditor()
{
	delete m_pixelUUIDHandler;
}

WorldEditor::PixelInfoHandler::PixelInfoHandler() {
}

WorldEditor::PixelInfoHandler::~PixelInfoHandler() {
	m_clickMouseBuffer.ReleaseAndGetAddressOf();
	m_UUIDOnMouseClickShader.ReleaseAndGetAddressOf();
	m_outputUUIDBuffer.ReleaseAndGetAddressOf();
	m_outputUUIDBufferStaged.ReleaseAndGetAddressOf();
	m_outputUUIDUAV.ReleaseAndGetAddressOf();
}

void WorldEditor::PixelInfoHandler::Init(ID3D11Device* device) {
	D3D11_BUFFER_DESC outputUUIDBufferDesc = {};
	ZeroMemory(&outputUUIDBufferDesc, sizeof(outputUUIDBufferDesc));
	outputUUIDBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	outputUUIDBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputUUIDBufferDesc.ByteWidth = sizeof(PixelInfo);
	outputUUIDBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	outputUUIDBufferDesc.StructureByteStride = sizeof(PixelInfo);
	device->CreateBuffer(&outputUUIDBufferDesc, nullptr,
		m_outputUUIDBuffer.GetAddressOf());

	outputUUIDBufferDesc.Usage = D3D11_USAGE_STAGING;
	outputUUIDBufferDesc.BindFlags = 0;
	outputUUIDBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	device->CreateBuffer(&outputUUIDBufferDesc, 0,
		m_outputUUIDBufferStaged.GetAddressOf());

	D3D11_UNORDERED_ACCESS_VIEW_DESC outputUUIDUAVDesc;
	outputUUIDUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	outputUUIDUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	outputUUIDUAVDesc.Buffer.FirstElement = 0;
	outputUUIDUAVDesc.Buffer.NumElements = 1;
	outputUUIDUAVDesc.Buffer.Flags = 0;
	device->CreateUnorderedAccessView(m_outputUUIDBuffer.Get(), &outputUUIDUAVDesc, m_outputUUIDUAV.GetAddressOf());

	CD3D11_BUFFER_DESC clickBufferDesc(2 * sizeof(uint64_t), D3D11_BIND_CONSTANT_BUFFER);
	HRESULT hr = device->CreateBuffer(&clickBufferDesc, nullptr, &m_clickMouseBuffer);
	if (FAILED(hr)) {
		printf("error!!!\n");
	}

	Microsoft::WRL::ComPtr<ID3DBlob> cs_blob;
	D3DCompileFromFile(JoinWchar_Wstring(EDITOR_ASSETS_DIR, L"Shaders/UUIDOnMouseClickCS.hlsl").c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "cs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&cs_blob, nullptr);
	device->CreateComputeShader(
		cs_blob->GetBufferPointer(),
		cs_blob->GetBufferSize(),
		nullptr,
		m_UUIDOnMouseClickShader.GetAddressOf()
	);
	return;
}

WorldEditor::PixelInfo WorldEditor::PixelInfoHandler::GetPixelInfo(ID3D11DeviceContext * context,
	eastl::shared_ptr<SE_G::GBuffer> gbuffer,
	UINT mouseClickX, UINT mouseClickY)
{
	ID3D11ShaderResourceView* UUIDTextureView = gbuffer->pUUIDSRV.Get();
	context->CSSetShaderResources(0u, 1u, &UUIDTextureView);
	ID3D11ShaderResourceView* WorldPosTextureView = gbuffer->pWorldPosSRV.Get();
	context->CSSetShaderResources(1u, 1u, &WorldPosTextureView);
	ID3D11ShaderResourceView* WorldNormalTextureView = gbuffer->pNormalSRV.Get();
	context->CSSetShaderResources(2u, 1u, &WorldNormalTextureView);

	UINT clickPos[2] = { mouseClickX, mouseClickY };
	context->UpdateSubresource(m_clickMouseBuffer.Get(), 0, nullptr, &clickPos, 0, 0);
	context->CSSetConstantBuffers(0, 1, m_clickMouseBuffer.GetAddressOf());

	UINT initCount[] = { (UINT)-1 };
	context->CSSetUnorderedAccessViews(0, 1, m_outputUUIDUAV.GetAddressOf(), initCount);

	context->CSSetShader(m_UUIDOnMouseClickShader.Get(), nullptr, 0);
	context->Dispatch(1, 1, 1);
	context->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView* uavs[] = { nullptr };
	context->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);
	ID3D11ShaderResourceView* nullSRVs[3] =
	{
		nullptr, nullptr, nullptr
	};
	context->CSSetShaderResources(0, 3, nullSRVs);

	context->CopyResource(m_outputUUIDBufferStaged.Get(), m_outputUUIDBuffer.Get());
	D3D11_MAPPED_SUBRESOURCE mappedData;
	context->Map(m_outputUUIDBufferStaged.Get(), 0, D3D11_MAP_READ, 0, &mappedData);

	PixelInfo* pixelInfo = reinterpret_cast<PixelInfo*>(mappedData.pData);

	// uint64_t uuid = (uint64_t)pixelInfo->hi << 32 | pixelInfo->lo;
	PixelInfo output;
	output = *pixelInfo;

	context->Unmap(m_outputUUIDBufferStaged.Get(), 0);

	// PixelInfo pixelInfo;
	// pixelInfo.uuid = SE::UUID(uuid);
	return output;
}

void WorldEditor::SetupRendering(
	eastl::shared_ptr<SE_G::RenderingSystem> renderSystem,
	UINT screenWidth,
	UINT screenHeight)
{
	this->m_screenWidth = screenWidth;
	this->m_screenHeight = screenHeight;

	this->m_renderingSystem = renderSystem.get();

	this->m_renderer = eastl::make_unique<SE_G::DeferredRenderer>(
		"WorldEditorDeferred", renderSystem->GetDevice(),
		renderSystem->GetDeviceContext(),
		m_screenWidth, m_screenHeight
	);
	this->m_renderer->InitParticleSystem();
	this->m_particleSystem = this->m_renderer->m_particleSystem.get();

	{
		m_gPass = static_cast<SE_G::GPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::GPass>(
				m_renderer.get(),
				m_renderer->m_GBuffer))
			);
	}
	{
		m_lightPass = static_cast<SE_G::LightPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::LightPass>(
				m_renderer.get(),
				m_renderer->m_GBuffer))
			);

		m_lightPass->m_particleSystem = m_renderer->m_particleSystem.get();
	}
	{
		m_colliderPass = static_cast<SE_G::ColliderPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::ColliderPass>(
				m_renderer.get(),
				m_renderer->m_GBuffer))
			);
	}
	{
		m_triggerPass = static_cast<SE_G::TriggerPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::TriggerPass>(
				m_renderer.get(),
				m_renderer->m_GBuffer))
			);
	}
	{
		m_emitterPass = static_cast<SE_G::EmitterDebugPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::EmitterDebugPass>(
				m_renderer.get(),
				m_renderer->m_GBuffer))
			);
	}
	{
		m_iconPass = static_cast<SE_G::IconPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::IconPass>(
				m_renderer.get(),
				m_renderer->m_GBuffer))
			);
	}
	{
		m_selectionPass = static_cast<SE_G::SelectionPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::SelectionPass>(
				m_renderer.get(),
				m_renderer->m_GBuffer))
			);
		m_selectionPass->m_iconPass = m_iconPass;
	}
	{
		m_perceptionPass = static_cast<SE_G::PerceptionDebugPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::PerceptionDebugPass>(
				m_renderer.get(),
				m_renderer->m_GBuffer))
			);
		m_selectionPass->m_perceptionPass = m_perceptionPass;
	}
	m_particleSystem->Enable();

	m_pixelUUIDHandler = new PixelInfoHandler();
	m_pixelUUIDHandler->Init(m_renderer->GetDevice());

	m_renderingSystem->AddRenderGroup(m_renderer.get());
}

void WorldEditor::InitMiniViewport()
{
	m_renderingSystem->RemoveRenderGroup("CharacterViewport");

	m_miniViewRenderer = eastl::make_shared<SE_G::MiniViewRenderer>(
		"CharacterViewport", m_renderer.get());
	m_miniViewRenderer->SetParentRenderer(m_renderer.get());
	m_miniViewRenderer->Disable();

	m_renderingSystem->AddRenderGroup(m_miniViewRenderer.get());
}

void WorldEditor::RenderMiniViewport()
{
	m_miniViewRenderer->Pass();
}

void WorldEditor::HandleKeyDown(Keys key)
{
	// In editor mode, use editor input manager
	m_editorInputManager.ProcessKeyDown(key);

	// Handle special editor keys
	if (key == Keys::RightButton) {
		IsRightMousePressed = true;
	}
}
void WorldEditor::HandleKeyUp(Keys key)
{
	// In editor mode, use editor input manager
	m_editorInputManager.ProcessKeyUp(key);

	// Handle special editor keys
	if (key == Keys::RightButton) {
		IsRightMousePressed = false;
	}
}

void WorldEditor::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	if (IsRightMousePressed)
	{
		m_renderer->m_mainCamera->RotateYaw(args.Offset.x * CameraRotateSpeed);
		m_renderer->m_mainCamera->RotatePitch(-args.Offset.y * CameraRotateSpeed);
	}

	if (args.WheelDelta != 0.0f)
	{
		CameraSpeed += ((args.WheelDelta > 0) - (args.WheelDelta < 0)) * CameraSpeedStep;

		if (CameraSpeed < MinCameraSpeed)
			CameraSpeed = MinCameraSpeed;
		else if (CameraSpeed > MaxCameraSpeed)
			CameraSpeed = MaxCameraSpeed;
	}
}

void WorldEditor::Start() {
	m_renderer->Enable();
	m_miniViewRenderer->Enable();
	m_particleSystem->Enable();
}

void WorldEditor::Pause() {
	m_renderer->Disable();
	m_miniViewRenderer->Disable();
	m_particleSystem->Disable();
}

void WorldEditor::Update(float deltaTime)
{
	if (m_particleSystem)
		m_particleSystem->Update(deltaTime);

	if (IsRightMousePressed)
	{
		// Use InputManager for camera movement (supports held keys)
		if (m_editorInputManager.IsKeyDown(Keys::W) || m_editorInputManager.IsKeyDown(Keys::S)) {
			float forward = (m_editorInputManager.IsKeyDown(Keys::W) ? 1.0f : 0.0f)
				- (m_editorInputManager.IsKeyDown(Keys::S) ? 1.0f : 0.0f);
			m_renderer->m_mainCamera->MoveForward(forward * CameraSpeed);
		}

		if (m_editorInputManager.IsKeyDown(Keys::D) || m_editorInputManager.IsKeyDown(Keys::A)) {
			float right = (m_editorInputManager.IsKeyDown(Keys::D) ? 1.0f : 0.0f)
				- (m_editorInputManager.IsKeyDown(Keys::A) ? 1.0f : 0.0f);
			m_renderer->m_mainCamera->MoveRight(right * CameraSpeed);
		}

		if (m_editorInputManager.IsKeyDown(Keys::E) || m_editorInputManager.IsKeyDown(Keys::Q)) {
			float up = (m_editorInputManager.IsKeyDown(Keys::E) ? 1.0f : 0.0f)
				- (m_editorInputManager.IsKeyDown(Keys::Q) ? 1.0f : 0.0f);
			m_renderer->m_mainCamera->MoveUp(up * CameraSpeed);
		}
	}
	//m_luaManager.Update(m_scene, deltaTime);
	//m_physicsSystem->Step(deltaTime);

	m_renderer->GetMainCamera()->Update(deltaTime);

	if (m_scene && m_miniViewRenderer && m_miniViewRenderer->IsEnabled())
	{
		m_miniViewRenderer->m_mainCamera->Update(deltaTime);
	}
	//scene->m_playerObject

	//m_playerObject->m_playerCamera->Update(deltaTime);
}

/*
void WorldEditor::SyncronizeTransforms() {
	m_physicsSystem->SyncronizeTransforms(&m_scene);
}
*/

void WorldEditor::CloseProject()
{
	ClearScene();
}

void WorldEditor::ClearScene() {
	//m_physicsSystem->ClearScene();
	if (m_scene)
		m_scene->ClearScene();
	m_renderer->ClearAllTechniques();
	m_renderer->RemovePass(SE_G::PassType::Shadow);
}

void WorldEditor::OnResize(UINT resizeWidth, UINT resizeHeight) {
	//m_renderer->GetMainCamera()->SetUpCameraViewByAspectRatio(m_screenWidth * 1.0f / m_screenHeight);
	if (resizeHeight == m_screenHeight)
	{
		m_renderer->GetMainCamera()->ResetCameraView(resizeWidth * 1.0f / resizeHeight);
		//m_renderer->GetMainCamera()->SetUpCameraViewByAspectRatio_horizontal(resizeWidth * 1.0f / resizeHeight);
	}
	else if (resizeWidth == m_screenWidth)
	{
		m_renderer->GetMainCamera()->ResetCameraView(resizeWidth * 1.0f / resizeHeight);
		//m_renderer->GetMainCamera()->SetUpCameraViewByAspectRatio_vertical(resizeWidth * 1.0f / resizeHeight);
	}
	else
	{
		m_renderer->GetMainCamera()->ResetCameraView(resizeWidth * 1.0f / resizeHeight);
	}

	m_screenWidth = resizeWidth;
	m_screenHeight = resizeHeight;

	m_renderer->OnResize(resizeWidth, resizeHeight);
}

WorldEditor::PixelInfo WorldEditor::GetPixelInfo(UINT x, UINT y)
{
	return m_pixelUUIDHandler->GetPixelInfo(m_renderer->GetDeviceContext(),
		m_renderer->m_GBuffer, x, y);
}

SE::UUID WorldEditor::ChooseObjectByClick(UINT x, UINT y)
{
	auto pixelInfo = m_pixelUUIDHandler->GetPixelInfo(m_renderer->GetDeviceContext(),
		m_renderer->m_GBuffer, x, y);
	uint64_t uuid = (uint64_t)pixelInfo.hi << 32 | pixelInfo.lo;
	return SE::UUID(uuid);
}

void WorldEditor::SaveScene(const wchar_t* scenePath)
{
	json j = m_scene->ToJson();
	std::ofstream file(scenePath);
	if (file) {
		file << j.dump(4);
		LOG_EDITOR_INFO("Scene saved");
	}
	else
		LOG_EDITOR_ERROR("Scene file output error");
}

bool WorldEditor::LoadScene(const wchar_t* scenePath) {
	std::ifstream file(scenePath);
	if (!file) {
		LOG_EDITOR_ERROR("File input error");
		return false;
	}
	json j;
	try {
		file >> j;
	}
	catch (const std::exception& e) {
		LOG_EDITOR_ERROR(JoinChar_String("JSON parse error: ", e.what()).c_str());
		return false;
	}
	m_scene = Scene_Info::FromJson(m_renderer.get(), j);
	
	LOG_EDITOR_INFO("Scene loaded");

	m_selectionPass->m_scene = m_scene.get();
 
	InitMiniViewport();

	return true;
}

void WorldEditor::SaveInputMapping(eastl::wstring inputMappingDir)
{
	json j = m_scene->m_keyMapping->ToJson();

	auto fullPath = inputMappingDir + Utf8ToWString(m_scene->m_keyMapping->m_name.c_str()) + L".json";

	std::ofstream file(fullPath.c_str());
	if (file) {
		file << j.dump(4);
		LOG_EDITOR_INFO("Input mapping saved");
	}
	else
		LOG_EDITOR_ERROR("Input mapping file output error");
}

bool WorldEditor::LoadInputMapping(eastl::wstring inputMappingDir)
{
	auto fullPath = inputMappingDir + Utf8ToWString(m_scene->m_keyMapping->m_name.c_str()) + L".json";

	std::ifstream file(fullPath.c_str());
	if (!file) {
		LOG_EDITOR_ERROR("File input error");
		return false;
	}
	json j;
	try {
		file >> j;
	}
	catch (const std::exception& e) {
		LOG_EDITOR_ERROR(JoinChar_String("JSON parse error: ", e.what()).c_str());
		return false;
	}
	m_scene->m_keyMapping->FromJson(j);

	LOG_EDITOR_INFO("Input mapping loaded");

	return true;
}

void WorldEditor::AddBoxShape(DXSM::Vector3 initPos)
{
	auto boxObject = EditorObjectFactory::CreateBoxObject(m_renderer.get());

	if (boxObject)
	{
		boxObject->GetComponent<TransformComponent_Info>()->m_assignedComponent->m_position = initPos;
		auto uuid = m_scene->AddGameObject(std::move(boxObject));
		m_scene->m_sceneGraph->Add(uuid);
	}
}

void WorldEditor::AddPlaneShape(DXSM::Vector3 initPos)
{
	auto planeObject = EditorObjectFactory::CreatePlaneObject(m_renderer.get());

	if (planeObject)
	{
		planeObject->GetComponent<TransformComponent_Info>()->m_assignedComponent->m_position = initPos;
		auto uuid = m_scene->AddGameObject(std::move(planeObject));
		m_scene->m_sceneGraph->Add(uuid);
	}
}

void WorldEditor::AddSphereShape(DXSM::Vector3 initPos)
{
	auto sphereObject = EditorObjectFactory::CreateSphereObject(m_renderer.get());

	if (sphereObject)
	{
		sphereObject->GetComponent<TransformComponent_Info>()->m_assignedComponent->m_position = initPos;
		auto uuid = m_scene->AddGameObject(std::move(sphereObject));
		m_scene->m_sceneGraph->Add(uuid);
	}
}

void WorldEditor::AddGeosphereShape(DXSM::Vector3 initPos)
{
	auto geosphereObject = EditorObjectFactory::CreateGeosphereObject(m_renderer.get());

	if (geosphereObject)
	{
		geosphereObject->GetComponent<TransformComponent_Info>()->m_assignedComponent->m_position = initPos;
		auto uuid = m_scene->AddGameObject(std::move(geosphereObject));
		m_scene->m_sceneGraph->Add(uuid);
	}
}

void WorldEditor::AddCylinderShape(DXSM::Vector3 initPos)
{
	auto cylinderObject = EditorObjectFactory::CreateCylinderObject(m_renderer.get());

	if (cylinderObject)
	{
		cylinderObject->GetComponent<TransformComponent_Info>()->m_assignedComponent->m_position = initPos;
		auto uuid = m_scene->AddGameObject(std::move(cylinderObject));
		m_scene->m_sceneGraph->Add(uuid);
	}
}

void WorldEditor::AddSkyBox(DXSM::Vector3 initPos)
{
	auto skyboxObject = EditorObjectFactory::CreateSkyBox(m_renderer.get());

	if (skyboxObject)
	{
		skyboxObject->GetComponent<TransformComponent_Info>()->m_assignedComponent->m_position = initPos;
		auto uuid = m_scene->AddGameObject(std::move(skyboxObject));
		m_scene->m_sceneGraph->Add(uuid);
	}
}

void WorldEditor::AddAmbientLight(DXSM::Vector3 initPos)
{
	auto ambientLightObject = EditorObjectFactory::CreateAmbientLightObject(m_renderer.get());

	if (ambientLightObject)
	{
		ambientLightObject->GetComponent<TransformComponent_Info>()->m_assignedComponent->m_position = initPos;
		auto uuid = m_scene->AddGameObject(std::move(ambientLightObject));
		m_scene->m_sceneGraph->Add(uuid);
	}
}

void WorldEditor::AddDirectionalLight(DXSM::Vector3 initPos)
{
	auto directionalLightObject = EditorObjectFactory::CreateDirectionalLightObject(m_renderer.get(),
		{
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3::Zero, 0,
			DXSM::Vector2(0, -DX::XM_PIDIV4), 0, 0
		});

	if (directionalLightObject)
	{
		directionalLightObject->GetComponent<TransformComponent_Info>()->m_assignedComponent->m_position = initPos;
		auto uuid = m_scene->AddGameObject(std::move(directionalLightObject));
		m_scene->m_sceneGraph->Add(uuid);
	}
}

void WorldEditor::AddPointLight(DXSM::Vector3 initPos)
{
	auto pointLightObject = EditorObjectFactory::CreatePointLightObject(m_renderer.get(),
		{
			DXSM::Vector3::One, 1.0f,
			DXSM::Vector3::One, 1.0f,
			DXSM::Vector3::Zero, 20,
			DXSM::Vector3::One, 0
		});

	if (pointLightObject)
	{
		pointLightObject->GetComponent<TransformComponent_Info>()->m_assignedComponent->m_position = initPos;
		auto uuid = m_scene->AddGameObject(std::move(pointLightObject));
		m_scene->m_sceneGraph->Add(uuid);
	}
}

void WorldEditor::AddSpotLight(DXSM::Vector3 initPos)
{
	auto spotLightObject = EditorObjectFactory::CreateSpotLightObject(m_renderer.get(),
		{
			DXSM::Vector3::One, 1.0f,
			DXSM::Vector3::One, 1.0f,
			DXSM::Vector3::Zero, 20,
			DXSM::Vector2(0, -DX::XM_PIDIV4), 10, 0,
			DXSM::Vector3::One, 0
		});

	if (spotLightObject)
	{
		spotLightObject->GetComponent<TransformComponent_Info>()->m_assignedComponent->m_position = initPos;
		auto uuid = m_scene->AddGameObject(std::move(spotLightObject));
		m_scene->m_sceneGraph->Add(uuid);
	}
}

void WorldEditor::AddCustomMesh(DXSM::Vector3 initPos)
{
	auto customMeshObject = EditorObjectFactory::CreateCustomMesh(
		m_renderer.get(),
		AssetPath(L"Box"));

	if (customMeshObject)
	{
		customMeshObject->GetComponent<TransformComponent_Info>()->m_assignedComponent->m_position = initPos;
		auto uuid = m_scene->AddGameObject(std::move(customMeshObject));
		m_scene->m_sceneGraph->Add(uuid);
	}
}


void WorldEditor::AddParticleEmitter(DXSM::Vector3 initPos)
{
	if (m_renderer && m_scene)
	{
		auto particleEmitter = EditorObjectFactory::CreateParticleEmitter(
			m_renderer->m_particleSystem.get());

		if (particleEmitter)
		{
			particleEmitter->GetComponent<TransformComponent_Info>()->m_assignedComponent->m_position = initPos;
			auto uuid = m_scene->AddGameObject(std::move(particleEmitter));
			m_scene->m_sceneGraph->Add(uuid);
		}
	}
	else
	{
		LOG_EDITOR_ERROR("Cannot add Custom Mesh: Renderer or Scene not initialized");
	}
}

/*
void WorldEditor::DeprojectScreenToWorld(DXSM::Vector2 mouseScreenCoords, DXSM::Vector2 lastGameViewportSize)
{
	float x = (2.0f * mouseScreenCoords.x) / lastGameViewportSize.x - 1.0f;
	float y = 1.0f - (2.0f * mouseScreenCoords.y) / lastGameViewportSize.y;
	float z = 0.0f;
	//printf("%f, %f :: %f, %f\n", mouseScreenCoords.x, mouseScreenCoords.y, lastGameViewportSize.x, lastGameViewportSize.y);

	DXSM::Vector4 ndcPosition(x, y, z, 1.0f);

	DXSM::Matrix viewProjMatrix =
		m_renderer->GetMainCamera()->GetViewMatrix() *
		m_renderer->GetMainCamera()->GetProjectionMatrix();
	DX::XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewProjMatrix);
	DXSM::Vector4 worldPos = XMVector3TransformCoord(ndcPosition, invViewProj);

	DXSM::Vector4 ndcPositionFar(x, y, 1.0f, 1.0f);
	DXSM::Vector4 worldPosFar = XMVector3TransformCoord(ndcPositionFar, invViewProj);

	rayDirection = worldPosFar - worldPos;
	rayDirection.Normalize();

	// auto trComp = m_scene->gameObjects[1]->GetComponent<TransformComponent>();
	// trComp->m_position = DXSM::Vector3(worldPos);
}
*/
