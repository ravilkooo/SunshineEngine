#include "Game.h"
#include <fstream>   // std::ofstream

#include <Scene.h>
#include <Physics/PhysicsSystem.h>
#include <Audio/AudioSystem.h>

#include <ParticleSystem/ParticleSystem.h>

#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/Pass/GPass.h>
#include <Graphics/Renderer/Pass/LightPass.h>
#include <Graphics/Renderer/Pass/ShadowMapPass.h>

#include <Graphics/Utils/Camera.h>

#include "AI/Perception/PerceptionSystem.h"
#include "AI/Behavior/BehaviorController.h"

#include <GameObject/GameObject.h>

#include <Component/PhysicsComponent.h>
#include <Component/TriggerComponent.h>
#include <Component/TransformComponent.h>
#include <Component/CameraComponent.h>

#include <ControllerSystem/CharacterControllerSystem.h>

#include <CameraManager.h>

#include <Utils/DebugUtils.h>

Game::Game()
{
	// Initialize();
	m_timer = GameTimer();
}

Game::~Game()
{
	// ������������ ��������
	m_audioSystem = nullptr;
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
				m_renderer.get(), m_renderer->m_GBuffer))
			);
	}
	{
		m_lightPass = static_cast<SE_G::LightPass*>(
			m_renderer->AddPass(eastl::make_unique<SE_G::LightPass>(
				m_renderer.get(), m_renderer->m_GBuffer))
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
		printSunshineErrorMessage("Scene file input error");
		//LOG_EDITOR_ERROR("File input error");
		return false;
	}
	json j;
	try {
		file >> j; // ��������� json �� �����
	}
	catch (const std::exception& e) {
		printSunshineErrorMessage(JoinChar_String("Scene JSON parse error: ", e.what()));
		//LOG_EDITOR_ERROR(JoinChar_String("JSON parse error: ", e.what()));
		return false;
	}

	m_physicsSystem = eastl::make_unique<PhysicsSystem>();

	Scene::FromJson(m_renderer.get(), m_physicsSystem.get(), j);

	m_renderer->SetMainCamera(
		Scene::GetInstance().m_cameraManager->GetCameraByUUID(
			Scene::GetInstance().m_mainCameraUUID));

	SetupPhysics();
	m_luaManager.InitializeBehavior();

	m_physicsSystem->FinalizeScene();
	
	InitializeAudio();

	m_characterControllerSystem = eastl::make_unique<CharacterControllerSystem>(&Scene::GetInstance(), m_physicsSystem.get());
	m_characterControllerSystem->InitCharacters();

	return true;
}

bool Game::LoadInputMapping(eastl::wstring inputMappingDir)
{
	auto fullPath = inputMappingDir + Utf8ToWString(PlayerInputSystem::GetInstance().m_keyMapping.m_name.c_str()) + L".json";

	std::ifstream file(fullPath.c_str());
	if (!file) {
		printSunshineErrorMessage("InputMapping file input error");
		// LOG_EDITOR_ERROR("File input error");
		return false;
	}
	json j;
	try {
		file >> j;
	}
	catch (const std::exception& e) {
		printSunshineErrorMessage(JoinChar_String("InputMapping JSON parse error: ", e.what()));
		// LOG_EDITOR_ERROR(JoinChar_String("JSON parse error: ", e.what()).c_str());
		return false;
	}
	PlayerInputSystem::GetInstance().FromJson(j);

	printSunshineMessage("Input mapping loaded");
	//LOG_EDITOR_INFO("Input mapping loaded");

	return true;
}

void Game::InitializeAudio()
{
	if (!AudioSystem::IsInitialized()) {
		m_audioSystem = &AudioSystem::Get();
		
		m_audioSystem->LoadFromJson("assets/config/audio_tracks.json");
		
		// audioSystem.Play("ambient_forest", 0.5f, true);
		// m_audioSystemPtr = &audioSystem;
	} else {
		m_audioSystem = &AudioSystem::Get();

		m_audioSystem->LoadFromJson("assets/config/audio_tracks.json");
	}
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

	m_characterControllerSystem->UpdateCharacters(deltaTime);
	m_characterControllerSystem->UpdateTriggerOverlaps();
	m_luaManager.Update(&Scene::GetInstance(), deltaTime);

	m_physicsSystem->FlushCommands();
	m_physicsSystem->Step(deltaTime);
	m_physicsSystem->FlushPreNextFrameCommands();

	m_physicsSystem->SyncronizeTransforms(&Scene::GetInstance(), deltaTime);

	if (m_particleSystem)
		m_particleSystem->Update(deltaTime);

	// m_playerObject->m_playerController.UpdatePlayer(deltaTime);

	// AI
	PerceptionSystem::Get().CheckSights(m_physicsSystem.get());
	BehaviorStorage::Get().Update(deltaTime);

	ClearCachedAbsoluteTransforms();

	m_renderer->GetMainCamera()->Update(deltaTime);

	if (AudioSystem::IsInitialized()) {
		AudioSystem::Get().Update();


		auto cam = Scene::GetInstance().m_cameraManager->GetCameraByUUID(Scene::GetInstance().m_mainCameraUUID);
		auto pos = cam->GetPosition();
		AudioSystem::Get().SetListenerPosition(pos.x, pos.y, pos.z);

	}

	PlayerInputSystem::GetInstance().EndFrame();
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
	PlayerInputSystem::GetInstance().HandleKeyDown(key);
}

void Game::HandleKeyUp(Keys key)
{
	PlayerInputSystem::GetInstance().HandleKeyUp(key);
}

void Game::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	PlayerInputSystem::GetInstance().HandleMouseMove(args);
}