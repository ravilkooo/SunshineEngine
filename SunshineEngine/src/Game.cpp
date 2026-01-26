#include "Game.h"
#include <fstream>   // std::ofstream
#include <PlayerObject/PlayerObject.h>

#include <Scene.h>

#include <ParticleSystem/ParticleSystem.h>
//#include <ParticleSystem/ParticleEmitterComponent.h>

#include <Graphics/Renderer/Pass/GPass.h>
#include <Graphics/Renderer/Pass/LightPass.h>
#include <Graphics/Renderer/Pass/ShadowMapPass.h>

#include "AI/Perception/PerceptionSystem.h"
#include "AI/Behavior/BehaviorController.h"

Game::Game()
{
	// Initialize();
	m_timer = GameTimer();
}

Game::~Game()
{
	// ������������ ��������
}

void Game::ClearScene()
{
	Scene::GetInstance().ClearScene();
	PerceptionSystem::Get().Clear();
	BehaviorStorage::Get().Clear();
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
	
	this->m_renderer->InitParticleSystem();
	this->m_particleSystem = this->m_renderer->m_particleSystem.get();
	
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

		m_lightPass->m_particleSystem = m_renderer->m_particleSystem.get();
	}
}

void Game::SetParticleSystem(eastl::shared_ptr <SE::ParticleSystem> ps)
{
	this->m_renderer->SetParticleSystem(ps);
	this->m_particleSystem = m_renderer->m_particleSystem.get();
	this->m_lightPass->m_particleSystem = m_renderer->m_particleSystem.get();
}

void Game::SetupPhysics()
{
	m_physicsSystem = eastl::make_unique<PhysicsSystem>();

	for (auto& it : Scene::GetInstance().uuidToObjectMap)
	{
		auto pc = it.second->GetComponent<PhysicsComponent>();
		if (pc)
			m_physicsSystem->CreateAndAddBody(pc.get());

		auto trigc = it.second->GetComponent<TriggerComponent>();
		if (trigc)
			m_physicsSystem->CreateAndAddTrigger(trigc.get());
	}
	m_physicsSystem->FinalizeScene();

	Scene::GetInstance().m_physicsSystem = m_physicsSystem.get();
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
		file >> j; // ��������� json �� �����
	}
	catch (const std::exception& e) {
		//LOG_EDITOR_ERROR(JoinChar_String("JSON parse error: ", e.what()));
		return false;
	}

	Scene::FromJson(m_renderer.get(), m_physicsSystem.get(), m_renderer->GetMainCamera(), j);
	m_playerObject = Scene::GetInstance().m_playerObject;
	SetupPhysics();
	m_luaManager.InitializeBehavior();
	return true;
}

bool Game::LoadGAIScene()
{
	SetupPhysics();

	// Add objects, add components, set parents

	Scene::GetInstance().RestoreParents();
	m_physicsSystem->FinalizeScene();
	return true;
}

bool Game::LoadDefaultScene()
{
	SetupPhysics();

	// Add objects, add components, set parents

	Scene::GetInstance().RestoreParents();
	m_physicsSystem->FinalizeScene();
	return true;
}

bool Game::LoadParentScene()
{
	
	SetupPhysics();

	// Add objects, add components, set parents

	Scene::GetInstance().RestoreParents();
	m_physicsSystem->FinalizeScene();
	return true;
}

bool Game::LoadLuaScene()
{
	
	SetupPhysics();

	// Add objects, add components, set parents

	Scene::GetInstance().RestoreParents();
	m_physicsSystem->FinalizeScene();
	return true;
}

bool Game::LoadResourcesScene()
{
	
	SetupPhysics();

	// Add objects, add components, set parents

	Scene::GetInstance().RestoreParents();
	m_physicsSystem->FinalizeScene();
	return true;
}


void Game::Start() {
	m_renderer->Enable();
	m_particleSystem->Enable();
}

void Game::Stop() {
	m_renderer->Disable();
	m_particleSystem->Disable();
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

void Game::ClearCachedAbsoluteTransforms()
{
	for (auto& it : Scene::GetInstance().uuidToObjectMap)
	{
		auto tc = it.second->GetComponent<TransformComponent>();
		if (tc)
		{
			tc->m_isAbsoluteTransformCached = false;
		}
	}
}

void Game::Update(float deltaTime) {

	Scene::GetInstance().FlushDestructionQueue();

	 m_luaManager.Update(&Scene::GetInstance(), deltaTime);

	 m_physicsSystem->Step(deltaTime);

	 m_physicsSystem->SyncronizeTransforms(&Scene::GetInstance());

	 if (m_particleSystem)
		 m_particleSystem->Update(deltaTime);

	 m_playerObject->m_playerController.UpdatePlayer(deltaTime);

	 // AI
	 PerceptionSystem::Get().CheckSights(m_physicsSystem.get());
	 BehaviorStorage::Get().Update(deltaTime);

	 ClearCachedAbsoluteTransforms();

	 m_renderer->GetMainCamera()->Update(deltaTime);
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


void Game::HandleKeyDown(Keys key)
{
	m_playerObject->m_playerController.HandleKeyDown(key);
}

void Game::HandleKeyUp(Keys key)
{
	m_playerObject->m_playerController.HandleKeyUp(key);
}

void Game::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	m_playerObject->m_playerController.HandleMouseMove(args);
}