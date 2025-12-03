#include "WorldEditor.h"
#include <Component/LuaComponent.h>
#include <Component/PhysicsComponent.h>
#include <fstream>   // std::ofstream

WorldEditor::WorldEditor()
{
}

WorldEditor::~WorldEditor()
{
	delete m_pixelUUIDHandler;
}


WorldEditor::PixelUUIDHandler::PixelUUIDHandler() {
}

WorldEditor::PixelUUIDHandler::~PixelUUIDHandler() {
	m_clickMouseBuffer.ReleaseAndGetAddressOf();
	m_UUIDOnMouseClickShader.ReleaseAndGetAddressOf();
	m_outputUUIDBuffer.ReleaseAndGetAddressOf();
	m_outputUUIDBufferStaged.ReleaseAndGetAddressOf();
	m_outputUUIDUAV.ReleaseAndGetAddressOf();
}

void WorldEditor::PixelUUIDHandler::Init(ID3D11Device* device) {
	D3D11_BUFFER_DESC outputUUIDBufferDesc;
	ZeroMemory(&outputUUIDBufferDesc, sizeof(outputUUIDBufferDesc));
	outputUUIDBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	outputUUIDBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputUUIDBufferDesc.ByteWidth = 4 * sizeof(UINT);
	//outputUUIDBufferDesc.MiscFlags = D3D1_RESOURCE_MISC_;
	device->CreateBuffer(&outputUUIDBufferDesc, nullptr,
		m_outputUUIDBuffer.GetAddressOf());

	outputUUIDBufferDesc.Usage = D3D11_USAGE_STAGING;
	outputUUIDBufferDesc.BindFlags = 0;
	outputUUIDBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	device->CreateBuffer(&outputUUIDBufferDesc, 0,
		m_outputUUIDBufferStaged.GetAddressOf());

	D3D11_UNORDERED_ACCESS_VIEW_DESC outputUUIDUAVDesc;
	outputUUIDUAVDesc.Format = DXGI_FORMAT_R32_UINT;
	outputUUIDUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	outputUUIDUAVDesc.Buffer.FirstElement = 0;
	outputUUIDUAVDesc.Buffer.NumElements = 4;
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

SE::UUID WorldEditor::PixelUUIDHandler::GetUUID(ID3D11DeviceContext* context,
	ID3D11ShaderResourceView* UUIDTextureView,
	UINT mouseClickX, UINT mouseClickY)
{
	context->CSSetShaderResources(0u, 1u, &UUIDTextureView);

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
	ID3D11ShaderResourceView* nullSRVs[] = { nullptr };
	context->CSSetShaderResources(0, 1, nullSRVs);

	context->CopyResource(m_outputUUIDBufferStaged.Get(), m_outputUUIDBuffer.Get());
	D3D11_MAPPED_SUBRESOURCE mappedData;
	context->Map(m_outputUUIDBufferStaged.Get(), 0, D3D11_MAP_READ, 0, &mappedData);

	uint32_t* hilo = reinterpret_cast<uint32_t*>(mappedData.pData);
	uint64_t uuid = (uint64_t)hilo[0] << 32 | hilo[1];

	context->Unmap(m_outputUUIDBufferStaged.Get(), 0);

	return SE::UUID(uuid);

	//return SE::UUID(0u);
}

void WorldEditor::SetupRendering(
	eastl::shared_ptr<SE_G::RenderingSystem> renderSystem,
	UINT screenWidth,
	UINT screenHeight)
{
	this->m_screenWidth = screenWidth;
	this->m_screenHeight = screenHeight;

	this->m_renderer = eastl::make_unique<SE_G::DeferredRenderer>(
		"WorldEditorDeferred", renderSystem->GetDevice(),
		renderSystem->GetDeviceContext(),
		m_screenWidth, m_screenHeight
	);

	{
		m_gPass = static_cast<SE_G::GPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::GPass>(
				m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
				m_renderer->m_GBuffer, m_renderer->GetMainCamera()))
			);
	}
	{
		m_lightPass = static_cast<SE_G::LightPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::LightPass>(
				m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
				m_renderer->m_GBuffer, m_renderer->GetMainCamera()))
			);
	}
	{
		m_colliderPass = static_cast<SE_G::ColliderPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::ColliderPass>(
				m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
				m_renderer->m_GBuffer, m_renderer->GetMainCamera()))
			);
	}
	{
		m_iconPass = static_cast<SE_G::IconPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::IconPass>(
				m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
				m_renderer->m_GBuffer, m_renderer->GetMainCamera()))
			);
	}
	{
		m_selectionPass = static_cast<SE_G::SelectionPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::SelectionPass>(
				m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
				m_renderer->m_GBuffer, m_renderer->GetMainCamera()))
			);
		m_selectionPass->m_iconPass = m_iconPass;
	}

	m_pixelUUIDHandler = new PixelUUIDHandler();
	m_pixelUUIDHandler->Init(m_renderer->GetDevice());
}

void WorldEditor::CreateParentTestScene()
{
	{
		this->m_scene = eastl::make_shared<Scene_Info>();

		SE::UUID boxId;

		m_scene->AddGameObject(EditorObjectFactory::CreateSkyBox(
			m_renderer.get(),
			m_renderer->GetMainCamera())
		);

		{
			boxId = m_scene->AddGameObject(
				EditorObjectFactory::CreateBoxObject(
					m_renderer.get(), 1.0f, 1.0f, 1.0f
				)
			);

			auto obj = m_scene->GetGameObjectByUUID(boxId);

			auto tc_info = obj->GetComponent<TransformComponent_Info>();
			auto rc_info = obj->GetComponent<RenderComponent_Info>();

			auto pc_info = obj->AddComponent<PhysicsComponent_Info>(rc_info.get(), tc_info.get());

			pc_info->SetCollisionLayer("MOVING");
			pc_info->SetMotion(SE::PhysicsMotionType::Dynamic);
			pc_info->SetActivation(SE::PhysicsActivation::Activate);
			pc_info->SetShape(SE::ColliderShapeType::Box);
			SE::ColliderSettings collSettings{};
			collSettings.data.asBox = { { 1.0f, 1.0f, 1.0f } };
			pc_info->m_colliderData->SetColliderSettings(collSettings);
		}

		for (size_t i = 0; i < 6; i++)
		{
			SE::UUID ballId = m_scene->AddGameObject(EditorObjectFactory::CreateSphereObject(
				m_renderer.get(), 1.0f)
			);
			auto obj = m_scene->GetGameObjectByUUID(m_scene->gameObjects.back());
			auto tc_info = obj->GetComponent<TransformComponent_Info>();

			tc_info->m_assignedComponent->m_position = DXSM::Vector3(
				3.0f * cos(DX::XM_2PI * i / 6.0f),
				3.0f * sin(DX::XM_2PI * i / 6.0f),
				0.0f);

			auto rc_info = obj->GetComponent<RenderComponent_Info>();

			auto pc_info = obj->AddComponent<PhysicsComponent_Info>(rc_info.get(), tc_info.get());

			pc_info->SetCollisionLayer("MOVING");
			pc_info->SetMotion(SE::PhysicsMotionType::Dynamic);
			pc_info->SetActivation(SE::PhysicsActivation::Activate);
			pc_info->SetShape(SE::ColliderShapeType::Sphere);
			SE::ColliderSettings collSettings{};
			collSettings.data.asSphere = { 1.0f };
			pc_info->m_colliderData->SetColliderSettings(collSettings);

			obj->SetParent({ boxId, m_scene->GetGameObjectByUUID(boxId), false });
		}

		m_scene->AddGameObject(EditorObjectFactory::CreateAmbientLightObject(
			m_renderer.get(),
			m_renderer->GetMainCamera(),
			{ DXSM::Vector3::One * 0.5f, 1.0f })
		);
		m_scene->AddGameObject(EditorObjectFactory::CreateDirectionalLightObject(
			m_renderer.get(),
			m_renderer->GetMainCamera(),
			{
				DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f) * 0.5f, 1.0f,
				DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f) * 0.5f, 1.0f,
				DXSM::Vector3::Zero, 0,
				DXSM::Vector3(1, -2, 0.5), 0
			})
		);
		m_scene->AddGameObject(EditorObjectFactory::CreatePointLightObject(
			m_renderer.get(),
			m_renderer->GetMainCamera(),
			{
				DXSM::Vector3(1.0f, 1.0f, 1.0f), 1.0f,
				DXSM::Vector3(1.0f, 1.0f, 1.0f), 1.0f,
				DXSM::Vector3(1.0f, 0.0f, 0.0f), 20,
				DXSM::Vector3(0.0f, 0.0f, 0.1f), 0
			})
		);

		// ----------------------------------------------------
		// Floor
		SE::UUID floorId;
		{
			floorId = m_scene->AddGameObject(EditorObjectFactory::CreateBoxObject(
				m_renderer.get(), 100.0f, 0.1f, 100.0f)
			);

			auto floorObj = m_scene->GetGameObjectByUUID(floorId);
			auto tc_info = floorObj->GetComponent<TransformComponent_Info>();
			tc_info->m_assignedComponent->m_position.y = -5.0f;

			auto rc_info = floorObj->GetComponent<RenderComponent_Info>();

			auto pc_info = floorObj->AddComponent<PhysicsComponent_Info>(rc_info.get(), tc_info.get());

			pc_info->SetCollisionLayer("NON_MOVING");
			pc_info->SetMotion(SE::PhysicsMotionType::Static);
			pc_info->SetActivation(SE::PhysicsActivation::DontActivate);
			pc_info->SetShape(SE::ColliderShapeType::Box);
			SE::ColliderSettings floorCollSettings{};
			floorCollSettings.data.asBox = { { 100.0f, 0.1f, 100.0f } };

			pc_info->m_colliderData->SetColliderSettings(floorCollSettings);
		}
		// ----------------------------------------------------

		// Ball
		SE::UUID secondBallId;
		{
			secondBallId = m_scene->AddGameObject(EditorObjectFactory::CreateSphereObject(
				m_renderer.get(), 0.5f)
			);

			auto obj = m_scene->GetGameObjectByUUID(secondBallId);

			auto tc_info = obj->GetComponent<TransformComponent_Info>();
			tc_info->m_assignedComponent->m_position.y = 2.0f;

			auto rc_info = obj->GetComponent<RenderComponent_Info>();

			auto pc_info = obj->AddComponent<PhysicsComponent_Info>(rc_info.get(), tc_info.get());

			pc_info->SetCollisionLayer("MOVING");
			pc_info->SetMotion(SE::PhysicsMotionType::Dynamic);
			pc_info->SetActivation(SE::PhysicsActivation::Activate);
			pc_info->SetShape(SE::ColliderShapeType::Capsule);
			SE::ColliderSettings collSettings{};
			collSettings.data.asCapsule = { 1.0f, 0.2f };

			pc_info->m_colliderData->SetColliderSettings(collSettings);
			
			obj->SetParent({ floorId, m_scene->GetGameObjectByUUID(floorId), false });
		}

		{
			SE::UUID customMeshId = m_scene->AddGameObject(
				EditorObjectFactory::CreateCustomMesh(
					m_renderer.get(), MakeEngineAssetPath_String("Meshes/plane.obj")
				)
			);
			auto obj = m_scene->GetGameObjectByUUID(customMeshId);

			obj->SetParent({ secondBallId, m_scene->GetGameObjectByUUID(secondBallId), false });
		}
		// ----------------------------------------------------

		//m_physicsSystem->FinalizeScene();
	}

	m_selectionPass->m_scene = m_scene.get();
}

void WorldEditor::CreateDefaultScene()
{
	{
		this->m_scene = eastl::make_shared<Scene_Info>();

		m_scene->AddGameObject(EditorObjectFactory::CreateSkyBox(
			m_renderer.get(),
			m_renderer->GetMainCamera())
		);

		{

			SE::UUID boxId = m_scene->AddGameObject(
				EditorObjectFactory::CreateBoxObject(
					m_renderer.get(), 1.0f, 1.0f, 1.0f
				)
			);

			auto obj = m_scene->GetGameObjectByUUID(boxId);

			auto tc_info = obj->GetComponent<TransformComponent_Info>();
			auto rc_info = obj->GetComponent<RenderComponent_Info>();

			auto pc_info = obj->AddComponent<PhysicsComponent_Info>(rc_info.get(), tc_info.get());

			pc_info->SetCollisionLayer("MOVING");
			pc_info->SetMotion(SE::PhysicsMotionType::Dynamic);
			pc_info->SetActivation(SE::PhysicsActivation::Activate);
			pc_info->SetShape(SE::ColliderShapeType::Box);
			SE::ColliderSettings collSettings{};
			collSettings.data.asBox = { { 1.0f, 1.0f, 1.0f } };
			pc_info->m_colliderData->SetColliderSettings(collSettings);
		}

		for (size_t i = 0; i < 6; i++)
		{
			SE::UUID ballId = m_scene->AddGameObject(EditorObjectFactory::CreateSphereObject(
				m_renderer.get(), 1.0f)
			);
			auto obj = m_scene->GetGameObjectByUUID(m_scene->gameObjects.back());
			auto tc_info = obj->GetComponent<TransformComponent_Info>();

			tc_info->m_assignedComponent->m_position = DXSM::Vector3(
				3.0f * cos(DX::XM_2PI * i / 6.0f),
				3.0f * sin(DX::XM_2PI * i / 6.0f),
				0.0f);

			auto rc_info = obj->GetComponent<RenderComponent_Info>();

			auto pc_info = obj->AddComponent<PhysicsComponent_Info>(rc_info.get(), tc_info.get());

			pc_info->SetCollisionLayer("MOVING");
			pc_info->SetMotion(SE::PhysicsMotionType::Dynamic);
			pc_info->SetActivation(SE::PhysicsActivation::Activate);
			pc_info->SetShape(SE::ColliderShapeType::Sphere);
			SE::ColliderSettings collSettings{};
			collSettings.data.asSphere = { 1.0f };
			pc_info->m_colliderData->SetColliderSettings(collSettings);
		}

		m_scene->AddGameObject(EditorObjectFactory::CreateAmbientLightObject(
			m_renderer.get(),
			m_renderer->GetMainCamera(),
			{ DXSM::Vector3::One * 0.5f, 1.0f })
		);
		m_scene->AddGameObject(EditorObjectFactory::CreateDirectionalLightObject(
			m_renderer.get(),
			m_renderer->GetMainCamera(),
			{
				DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f) * 0.5f, 1.0f,
				DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f) * 0.5f, 1.0f,
				DXSM::Vector3::Zero, 0,
				DXSM::Vector3(1, -2, 0.5), 0
			})
		);
		m_scene->AddGameObject(EditorObjectFactory::CreatePointLightObject(
			m_renderer.get(),
			m_renderer->GetMainCamera(),
			{
				DXSM::Vector3(1.0f, 1.0f, 1.0f), 1.0f,
				DXSM::Vector3(1.0f, 1.0f, 1.0f), 1.0f,
				DXSM::Vector3(1.0f, 0.0f, 0.0f), 20,
				DXSM::Vector3(0.0f, 0.0f, 0.1f), 0
			})
		);

		// ----------------------------------------------------
		// Floor
		{
			SE::UUID floorId = m_scene->AddGameObject(EditorObjectFactory::CreateBoxObject(
				m_renderer.get(), 100.0f, 0.1f, 100.0f)
			);

			auto floorObj = m_scene->GetGameObjectByUUID(floorId);
			auto tc_info = floorObj->GetComponent<TransformComponent_Info>();
			tc_info->m_assignedComponent->m_position.y = -5.0f;

			auto rc_info = floorObj->GetComponent<RenderComponent_Info>();

			auto pc_info = floorObj->AddComponent<PhysicsComponent_Info>(rc_info.get(), tc_info.get());

			pc_info->SetCollisionLayer("NON_MOVING");
			pc_info->SetMotion(SE::PhysicsMotionType::Static);
			pc_info->SetActivation(SE::PhysicsActivation::DontActivate);
			pc_info->SetShape(SE::ColliderShapeType::Box);
			SE::ColliderSettings floorCollSettings{};
			floorCollSettings.data.asBox = { { 100.0f, 0.1f, 100.0f } };

			pc_info->m_colliderData->SetColliderSettings(floorCollSettings);
		}
		// ----------------------------------------------------

		// Ball
		{
			SE::UUID ballId = m_scene->AddGameObject(EditorObjectFactory::CreateSphereObject(
				m_renderer.get(), 0.5f)
			);

			auto obj = m_scene->GetGameObjectByUUID(ballId);

			auto tc_info = obj->GetComponent<TransformComponent_Info>();
			tc_info->m_assignedComponent->m_position.y = 2.0f;

			auto rc_info = obj->GetComponent<RenderComponent_Info>();

			auto pc_info = obj->AddComponent<PhysicsComponent_Info>(rc_info.get(), tc_info.get());

			pc_info->SetCollisionLayer("MOVING");
			pc_info->SetMotion(SE::PhysicsMotionType::Dynamic);
			pc_info->SetActivation(SE::PhysicsActivation::Activate);
			pc_info->SetShape(SE::ColliderShapeType::Capsule);
			SE::ColliderSettings collSettings{};
			collSettings.data.asCapsule = { 1.0f, 0.2f };

			pc_info->m_colliderData->SetColliderSettings(collSettings);
		}

		{
			SE::UUID customMeshId = m_scene->AddGameObject(
				EditorObjectFactory::CreateCustomMesh(
					m_renderer.get(), MakeEngineAssetPath_String("Meshes/plane.obj")
				)
			);
		}
		// ----------------------------------------------------

		//m_physicsSystem->FinalizeScene();
	}

	m_selectionPass->m_scene = m_scene.get();
}

void WorldEditor::Start() {
	m_renderer->Enable();
}

void WorldEditor::Pause() {
	m_renderer->Disable();
}

void WorldEditor::Update(float deltaTime) {

	//m_luaManager.Update(m_scene, deltaTime);
	//m_physicsSystem->Step(deltaTime);
}

/*
void WorldEditor::SyncronizeTransforms() {
	m_physicsSystem->SyncronizeTransforms(&m_scene);
}
*/

void WorldEditor::Render() {
	
}

void WorldEditor::ClearScene() {
	//m_physicsSystem->ClearScene();
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

SE::UUID WorldEditor::ChooseObjectByClick(UINT x, UINT y)
{
	return SE::UUID(m_pixelUUIDHandler->GetUUID(m_renderer->GetDeviceContext(),
		m_renderer->m_GBuffer->pUUIDSRV.Get(), x, y));
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
		LOG_EDITOR_ERROR("File output error");
}

bool WorldEditor::LoadScene(const wchar_t* scenePath) {
	std::ifstream file(scenePath);
	if (!file) {
		LOG_EDITOR_ERROR("File input error");
		return false;
	}
	json j;
	try {
		file >> j; // ��������� json �� �����
	}
	catch (const std::exception& e) {
		LOG_EDITOR_ERROR(JoinChar_String("JSON parse error: ", e.what()).c_str());
		return false;
	}
	m_scene = Scene_Info::FromJson(m_renderer.get(), m_renderer->GetMainCamera(), j);
	/*
	if (!loadedScene) {
		LOG_EDITOR_ERROR("Scene load error\n");
		return false;
	}
	*/
	LOG_EDITOR_INFO("Scene loaded");

	m_selectionPass->m_scene = m_scene.get();


	return true;
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


