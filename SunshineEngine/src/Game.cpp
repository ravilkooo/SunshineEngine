#include "Game.h"
#include <fstream>   // std::ofstream
#include <Graphics/Renderer/Pass/ShadowMapPass.h>
#include <PlayerObject/PlayerObject.h>

Game::Game()
{
	//Initialize();
}

Game::~Game()
{
	// Освобождение ресурсов
}

void Game::SetupRendering(
	eastl::shared_ptr<SE_G::RenderingSystem> renderSystem,
	UINT screenWidth,
	UINT screenHeight)
{
	this->m_screenHeight = screenHeight;
	this->m_screenWidth = screenWidth;

	this->m_renderer = eastl::make_unique<SE_G::DeferredRenderer>(
		"GameDeferred", renderSystem->GetDevice(),
		renderSystem->GetDeviceContext(),
		m_screenWidth, m_screenHeight);

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
}

void Game::SetupPhysics()
{
	m_physicsSystem = eastl::make_unique<PhysicsSystem>();
	// For Volodya
	//m_tracingSystem = eastl::make_unique<TracingSystem>();
}

bool Game::LoadScene(const wchar_t* scenePath)
{
	std::ifstream file(scenePath);
	if (!file) {
		//LOG_EDITOR_ERROR("File input error");
		return false;
	}
	json j;
	try {
		file >> j; // прочитать json из файла
	}
	catch (const std::exception& e) {
		//LOG_EDITOR_ERROR(JoinChar_String("JSON parse error: ", e.what()));
		return false;
	}

	SetupPhysics();
	m_scene = Scene::FromJson(m_renderer.get(), m_physicsSystem.get(), m_renderer->GetMainCamera(), j);
	m_playerObject = m_scene->m_playerObject;
	/*
	if (!loadedScene) {
		LOG_EDITOR_ERROR("Scene load error\n");
		return false;
	}
	*/
	//LOG_EDITOR_INFO("Scene loaded");

	// For Volodya
	/*
	auto m_uuid0 = m_scene->gameObjects[0];
	auto m_gobj0 = m_scene->GetGameObjectByUUID(m_uuid0);
	auto m_uuid1 = m_scene->gameObjects[1];
	auto m_gobj1 = m_scene->GetGameObjectByUUID(m_uuid1);

	TracedBody* tb0 = new TracedBody(m_uuid0, m_gobj0->GetComponent<TransformComponent>().get());

	tb0->m_objectLayer = 0u;
	tb0->m_motionType = JPH::EMotionType::Dynamic;
	tb0->m_activation = JPH::EActivation::Activate;
	JPH::ShapeSettings::ShapeResult shapeResult;
	JPH::BoxShapeSettings boxSettings(
		JPH::Vec3(
			0.5f,
			0.5f,
			0.5f
		)
	);
	shapeResult = boxSettings.Create();
	tb0->m_shape = shapeResult.Get();
	m_tracingSystem->CreateAndAddBody(tb0);

	TracedBody* tb1 = new TracedBody(m_uuid0, m_gobj0->GetComponent<TransformComponent>().get());
	tb1->m_objectLayer = 0u;
	tb1->m_motionType = JPH::EMotionType::Dynamic;
	tb1->m_activation = JPH::EActivation::Activate;
	tb1->m_shape = shapeResult.Get();
	m_tracingSystem->CreateAndAddBody(tb1);

	m_tracingSystem->FinalizeScene();
	*/

	m_physicsSystem->FinalizeScene();
	return true;
}

bool Game::LoadGAIScene()
{
	auto scene = eastl::make_shared<Scene>();
	SetupPhysics();

	// Add objects, add components, set parents

	scene->RestoreParents();
	m_physicsSystem->FinalizeScene();
	return true;
}

bool Game::LoadDefaultScene()
{
	auto scene = eastl::make_shared<Scene>();
	SetupPhysics();

	// Add objects, add components, set parents

	scene->RestoreParents();
	m_physicsSystem->FinalizeScene();
	return true;
}

bool Game::LoadParentScene()
{
	auto scene = eastl::make_shared<Scene>();
	SetupPhysics();

	// Add objects, add components, set parents

	scene->RestoreParents();
	m_physicsSystem->FinalizeScene();
	return true;
}

bool Game::LoadLuaScene()
{
	auto scene = eastl::make_shared<Scene>();
	SetupPhysics();

	// Add objects, add components, set parents

	scene->RestoreParents();
	m_physicsSystem->FinalizeScene();
	return true;
}

bool Game::LoadResourcesScene()
{
	auto scene = eastl::make_shared<Scene>();
	SetupPhysics();

	// Add objects, add components, set parents

	scene->RestoreParents();
	m_physicsSystem->FinalizeScene();
	return true;
}


void Game::Start() {
	m_renderer->Enable();
}

void Game::Stop() {
	m_renderer->Disable();
}

void Game::Run()
{
	/*
	float physicsUpdateFPS = 120.0f;
	float physicsUpdateMs = 1.0f / physicsUpdateFPS;
	float accumulator = 0.0f;
	float accumulatorLimit = 4.0f * physicsUpdateMs;

	MSG msg = {};
	bool isExitRequested = false;
	
	// FPS statitistic
	unsigned int frameCount = 0;
	float FPSstatisticTimer = 0;

	while (!isExitRequested) {
		// Handle the windows messages.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			// If windows signals to end the application then exit out.
			if (msg.message == WM_QUIT) {
				isExitRequested = true;
			}
			continue;
		}


		m_timer.Tick();
		m_deltaTime = m_timer.GetDeltaTime();
		accumulator += m_deltaTime;
		accumulator = eastl::min(4.0f * physicsUpdateMs, accumulator);
	
		// FPS statistic
		FPSstatisticTimer += m_deltaTime;
		frameCount++;
		if (FPSstatisticTimer > 1.0f) {
			float fps = frameCount * 1.0f / FPSstatisticTimer;

			FPSstatisticTimer -= 1.0f;

			WCHAR text[256];
			swprintf_s(text, TEXT("FPS: %f"), fps);
			SetWindowText(m_displayWindow.m_hWnd, text);

			frameCount = 0;
		}


		while (accumulator >= physicsUpdateMs) {
			Update(physicsUpdateMs);
			accumulator -= physicsUpdateMs;
		}
		Render();
	}
	*/
}

void Game::Update(float deltaTime) {

	 m_luaManager.Update(m_scene.get(), deltaTime);
	 m_playerObject->m_playerController.UpdatePlayer(deltaTime);

	 m_physicsSystem->Step(deltaTime);

	 m_physicsSystem->SyncronizeTransforms(m_scene.get());
	 
	 // For Volodya
	 //m_tracingSystem->SyncronizeTransforms(m_scene.get());
}

void Game::Render()
{
	// m_renderer->RenderScene(m_scene);
}

void Game::OnResize(UINT resizeWidth, UINT resizeHeight) {
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
