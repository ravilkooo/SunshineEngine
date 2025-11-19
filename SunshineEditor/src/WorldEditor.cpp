#include "WorldEditor.h"
#include <Component/LuaComponent.h>
#include <fstream>   // std::ofstream

WorldEditor::WorldEditor()
{
}

WorldEditor::~WorldEditor()
{
	m_physicsSystem->ClearScene();
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

void WorldEditor::InitScene()
{
	//m_physicsSystem = eastl::make_shared<PhysicsSystem>();

	/*
	TestObjects
	*/
	// To-do: there should be path to opened project
	// to-do: class Project
	if (!LoadScene(WcharToChar(JoinWchar_Wchar(PROJECTS_DIR, L"DefaultScene/scene.json"))))
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

			/*
			auto physicsComp = m_scene->GetGameObjectByUUID(boxId)->AddComponent<PhysicsComponent>();

			physicsComp->SetObjecUUID(boxId);
			physicsComp->SetObjectLayer(Layers::MOVING); // Example ObjectLayer
			physicsComp->SetPosition(JPH::RVec3(0.0f, 0.0f, 0.0f));
			auto quat = DXSM::Quaternion::CreateFromYawPitchRoll(0.4f, 0.3f, 0.333f);
			physicsComp->SetOrientation(JPH::Quat(quat.x, quat.y, quat.z, quat.w));
			physicsComp->SetMotionType(JPH::EMotionType::Dynamic);
			physicsComp->SetActivation(JPH::EActivation::Activate);

			// Create a box shape for the physics body
			JPH::BoxShapeSettings boxSettings(JPH::Vec3(0.5f, 0.5f, 0.5f));
			boxSettings.SetEmbedded();
			JPH::ShapeRefC boxShape = boxSettings.Create().Get();

			// Set the shape to the component
			physicsComp->SetShape(boxShape);

			physicsComp->CreateBody(m_physicsSystem);
			*/
		}

		for (size_t i = 0; i < 6; i++)
		{
			SE::UUID ballId = m_scene->AddGameObject(EditorObjectFactory::CreateSphereObject(
				m_renderer.get(), 1.0f)
			);
			auto obj = m_scene->GetGameObjectByUUID(m_scene->gameObjects.back());
			auto tr_info = obj->GetComponent<TransformComponent_Info>();

			tr_info->m_assignedComponent->m_position = DXSM::Vector3(
				3.0f * cos(DX::XM_2PI * i / 6.0f),
				3.0f * sin(DX::XM_2PI * i / 6.0f),
				0.0f);

			/*
			auto physicsComp = m_scene->GetGameObjectByUUID(ballId)->AddComponent<PhysicsComponent>();

			physicsComp->SetObjecUUID(ballId);
			physicsComp->SetObjectLayer(Layers::MOVING); // Example ObjectLayer
			physicsComp->SetPosition(JPH::RVec3(
				3.0f * cos(DX::XM_2PI * i / 6.0f),
				3.0f * sin(DX::XM_2PI * i / 6.0f),
				0.0f));
			physicsComp->SetOrientation(JPH::Quat::sIdentity());
			physicsComp->SetMotionType(JPH::EMotionType::Dynamic);
			physicsComp->SetActivation(JPH::EActivation::Activate);

			// Create a box shape for the physics body
			JPH::SphereShapeSettings settings(1.0f);
			settings.SetEmbedded();
			JPH::ShapeRefC sphereShape = settings.Create().Get();

			// Set the shape to the component
			physicsComp->SetShape(sphereShape);

			physicsComp->CreateBody(m_physicsSystem);
			*/
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
			/*
			auto physicsComp = m_scene->GetGameObjectByUUID(floorId)->AddComponent<PhysicsComponent>();

			physicsComp->SetObjecUUID(floorId);
			physicsComp->SetObjectLayer(Layers::NON_MOVING); // Example ObjectLayer
			physicsComp->SetPosition(JPH::RVec3(0.0f, -5.0f, 0.0f));
			physicsComp->SetOrientation(JPH::Quat::sIdentity());
			physicsComp->SetMotionType(JPH::EMotionType::Static);
			physicsComp->SetActivation(JPH::EActivation::DontActivate);

			// Create a box shape for the physics body
			JPH::BoxShapeSettings boxSettings(JPH::Vec3(50.0f, 0.05f, 50.0f));
			boxSettings.SetEmbedded();
			JPH::ShapeRefC boxShape = boxSettings.Create().Get();

			// Set the shape to the component
			physicsComp->SetShape(boxShape);

			physicsComp->CreateBody(m_physicsSystem);
			*/

			auto tr_info = m_scene->GetGameObjectByUUID(floorId)->GetComponent<TransformComponent_Info>();

			tr_info->m_assignedComponent->m_position.y = -5.0f;
		}
		// ----------------------------------------------------

		// Ball
		{
			SE::UUID ballId = m_scene->AddGameObject(EditorObjectFactory::CreateSphereObject(
				m_renderer.get(), 0.5f)
			);

			/*
			auto physicsComp = m_scene->GetGameObjectByUUID(ballId)->AddComponent<PhysicsComponent>();

			physicsComp->SetObjecUUID(ballId);
			physicsComp->SetObjectLayer(Layers::MOVING); // Example ObjectLayer
			physicsComp->SetPosition(JPH::RVec3(0.0f, 2.0f, 0.0f));
			physicsComp->SetOrientation(JPH::Quat::sIdentity());
			physicsComp->SetMotionType(JPH::EMotionType::Dynamic);
			physicsComp->SetActivation(JPH::EActivation::Activate);

			// Create a box shape for the physics body
			JPH::SphereShapeSettings settings(0.5f);
			settings.SetEmbedded();
			JPH::ShapeRefC sphereShape = settings.Create().Get();

			// Set the shape to the component
			physicsComp->SetShape(sphereShape);

			physicsComp->CreateBody(m_physicsSystem);
			*/
			auto tr_info = m_scene->GetGameObjectByUUID(ballId)->GetComponent<TransformComponent_Info>();
			tr_info->m_assignedComponent->m_position.y = 2.0f;
		}
		// ----------------------------------------------------

		//m_physicsSystem->FinalizeScene();
	}

	m_selectionPass->m_scene = m_scene;
}

void WorldEditor::Start() {
	m_renderer->Enable();
}

void WorldEditor::Pause() {
	m_renderer->Disable();
}

void WorldEditor::Update(float deltaTime) {

	m_luaManager.Update(m_scene, deltaTime);
	//m_scene.gameObjects[1]->GetComponent<TransformComponent>()->m_localRotation.y += deltaTime;
	// m_scene.gameObjects[1]->GetComponent<TransformComponent>()->m_position.x += rayDirection.x * deltaTime * 10.0f;
	// m_scene.gameObjects[1]->GetComponent<TransformComponent>()->m_position.y += rayDirection.y * deltaTime * 10.0f;
	// m_scene.gameObjects[1]->GetComponent<TransformComponent>()->m_position.z += rayDirection.z * deltaTime * 10.0f;
	m_physicsSystem->Step(deltaTime);
	
	/*
	auto ballPos = physSystem->SpherePosition();

	m_scene.GetGameObjectByUUID(ballId)->GetComponent<TransformComponent>()->m_position =
	{ballPos.GetX(), ballPos.GetY(), ballPos.GetZ()};
	*/
}

void WorldEditor::SyncronizeTransforms() {
	m_physicsSystem->SyncronizeTransforms(&m_scene);
}

void WorldEditor::Render() {
	
}

void WorldEditor::ClearScene() {
	m_physicsSystem->ClearScene();
}

void WorldEditor::OnResize(UINT resizeWidth, UINT resizeHeight) {
	//m_renderer->GetMainCamera()->SetUpCameraViewByAspectRatio(m_screenWidth * 1.0f / m_screenHeight);
	if (resizeHeight == m_screenHeight)
		m_renderer->GetMainCamera()->SetUpCameraViewByAspectRatio_horizontal(resizeWidth * 1.0f / resizeHeight);
	else if (resizeWidth == m_screenWidth)
		m_renderer->GetMainCamera()->SetUpCameraViewByAspectRatio_vertical(resizeWidth * 1.0f / resizeHeight);
	else
		m_renderer->GetMainCamera()->ResetCameraView(resizeWidth * 1.0f / resizeHeight);

	m_screenWidth = resizeWidth;
	m_screenHeight = resizeHeight;

	m_renderer->OnResize(resizeWidth, resizeHeight);
}

SE::UUID WorldEditor::ChooseObjectByClick(UINT x, UINT y)
{
	return SE::UUID(m_pixelUUIDHandler->GetUUID(m_renderer->GetDeviceContext(),
		m_renderer->m_GBuffer->pUUIDSRV.Get(), x, y));
}

void WorldEditor::SaveScene(const std::string& scenePath)
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

bool WorldEditor::LoadScene(const std::string& scenePath) {
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
		LOG_EDITOR_ERROR(JoinChar_Char("JSON parse error: ", e.what()));
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

	// auto trComp = m_scene.gameObjects[1]->GetComponent<TransformComponent>();
	// trComp->m_position = DXSM::Vector3(worldPos);
}
*/
