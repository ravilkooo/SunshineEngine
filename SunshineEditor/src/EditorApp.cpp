#include "EditorApp.h"
#include "Utils/DebugUtils.h"
#include <fstream>   // std::ofstream
#include <filesystem>
#include <Utils/AssetPath.h>

#include <ParticleSystem/ParticleSystem.h>
#include <ResourceManager/ResourceManagerFacade.h>
#include <ResourceManager/ResourceLoader/TextureLoader.h>

#include <Graphics/Renderer/Pass/RenderPass.h>
#include <Graphics/Renderer/Pass/LightPass.h>

#include <ImguiEditorPass.h>

#include <Project.h>

EditorApp::EditorApp()
{

}

void EditorApp::InitEditorApp(UINT winWidth, UINT winHeight)
{
	m_applicationName = L"SunshineEngine";
	m_hInstance = GetModuleHandle(nullptr);

	m_winWidth = winWidth;
	m_winHeight = winHeight;

	// m_timer = GameTimer();

	m_displayWindow = DisplayWindow(this, m_applicationName, m_hInstance,
		m_winWidth, m_winHeight, DisplayWindow::WndProcImGui);

	m_renderingSystem = eastl::make_shared<SE_G::RenderingSystem>(
		m_displayWindow.m_hWnd,
		m_winWidth, m_winHeight);

	if (!AudioSystem::IsInitialized()) {
		AudioSystem& audioSystem = AudioSystem::Get();
	}
	m_audioEditor = eastl::make_unique<AudioEditor>(&AudioSystem::Get());
	
	m_audioEditor->LoadFromJson();
	
	UINT worldEditorWidth = winWidth / 2;
	UINT worldEditorHeight = winHeight / 2;

	// Init WorldEditor with all it's passes
	m_worldEditor = eastl::make_shared<WorldEditor>();
	m_worldEditor->SetupRendering(m_renderingSystem, worldEditorWidth, worldEditorHeight);
	InitResourceLoaders(m_renderingSystem->GetDevice());

	// Init lua/sol2 state
	m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::table, sol::lib::math);
	sol_ImGui::Init(m_lua);

	// Init imgui staff
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// enable docking
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(m_displayWindow.m_hWnd);
	ImGui_ImplDX11_Init(
		m_renderingSystem->GetDevice(),
		m_renderingSystem->GetDeviceContext());

	// Imgui Pass
	m_imguiRenderGroup = eastl::make_unique<SE_G::RenderGroup>(
		"ImguiEditor", m_renderingSystem->GetDevice(),
		m_renderingSystem->GetDeviceContext()
	);
	m_renderingSystem->AddRenderGroup(m_imguiRenderGroup.get());

	imguiEditorPass = static_cast<ImguiEditorPass*>(
		m_imguiRenderGroup->AddPass(
			eastl::make_unique<ImguiEditorPass>(this)));

	imguiEditorPass->SetVieportGBuffer(
		m_worldEditor->m_renderer->m_GBuffer.get());

	imguiEditorPass->SetAudioEditor(m_audioEditor.get());

	m_initialized = true;

	m_runtimeMode = RuntimeMode::WORLD_EDITOR_MODE;

	// Show window
	// ShowWindow(m_displayWindow.m_hWnd, SW_SHOWDEFAULT);
	// UpdateWindow(m_displayWindow.m_hWnd);

	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &EditorApp::HandleKeyDown);
	InputDevice::getInstance().OnKeyReleased.AddRaw(this, &EditorApp::HandleKeyUp);
	InputDevice::getInstance().MouseMove.AddRaw(this, &EditorApp::HandleMouseMove);
}

EditorApp::~EditorApp() {
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (AudioSystem::IsInitialized()) {
		AudioSystem::Destroy();
	}
}

void EditorApp::RunApp()
{
	SE::Project::SetWorldEditor(m_worldEditor.get());

	// Fixed timestep physics update
	physicsUpdateFPS = 120.0f;
	physicsUpdateMs = 1.0f / physicsUpdateFPS;
	accumulator = 0.0f;
	accumulatorLimit = 4.0f * physicsUpdateMs;

	// FPS statitistic
	frameCount = 0;
	FPSstatisticTimer = 0;

	MSG msg = {};
	bool isExitRequested = false;
	
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

		if (m_runtimeMode == RuntimeMode::WORLD_EDITOR_MODE)
		{
			m_worldEditor->m_timer.Tick();
			m_deltaTime = m_worldEditor->m_timer.GetDeltaTime();

			accumulator += m_deltaTime;
			accumulator = eastl::min(accumulatorLimit, accumulator);
		}
		else if (m_runtimeMode == RuntimeMode::GAME_MODE && !m_gamePaused)
		{
			m_currentGame->m_timer.Tick();
			m_deltaTime = m_currentGame->m_timer.GetDeltaTime();

			accumulator += m_deltaTime;
			accumulator = eastl::min(accumulatorLimit, accumulator);
		}
		else
		{
			accumulator = 0.0f;
		}

		// FPS statistic
		FPSstatisticTimer += m_deltaTime;
		frameCount++;
		if (FPSstatisticTimer > 1.0f) {
			float fps = frameCount * 1.0f / FPSstatisticTimer;

			FPSstatisticTimer -= 1.0f;

			WCHAR text[256];

			if (m_projectSelected)
			{
				if (m_loadedSceneType == SE::SceneType::Custom)
					swprintf_s(text, TEXT("SunshineEngine: [%ls]   FPS: %f"), m_openedProject->GetSubPath().c_str(), fps);
				else
				{
					const char* sceneName = SE::ProjectSelector::SceneTypeToDisplayName(m_loadedSceneType);
					swprintf_s(text, TEXT("SunshineEngine: %hs - FPS: %.1f"), sceneName, fps);
				}
					
			}
			else
			{
				swprintf_s(text, TEXT("SunshineEngine: Project Selector - FPS: %.1f"), fps);
			}

			SetWindowText(m_displayWindow.m_hWnd, text);

			frameCount = 0;
		}


		if (m_runtimeMode == RuntimeMode::WORLD_EDITOR_MODE)
		{
			while (accumulator >= physicsUpdateMs) {
				// UpdateGame(physicsUpdateMs);
				UpdateEditor(physicsUpdateMs);
				accumulator -= physicsUpdateMs;
			}
		}
		else
		{
			if (!m_gamePaused)
			{
				while (accumulator >= physicsUpdateMs) {
					UpdateGame(physicsUpdateMs);
					accumulator -= physicsUpdateMs;
				}
			}
		}
		
		Render();

		if (!m_projectSelected && imguiEditorPass->IsProjectSelected()) {
			if (OpenProject()) {
				m_projectSelected = true;
				m_worldEditor->m_scene->InitHierarchy();
				imguiEditorPass->SetScene(m_worldEditor->m_scene);
			} else {
				imguiEditorPass->ResetProjectSelection();
				m_openedProject = nullptr;
			}
		}

		if (!m_projectSelected && !imguiEditorPass->IsProjectSelectorVisible()) {
			if (imguiEditorPass->IsProjectSelectorVisible())
			{
				imguiEditorPass->ResetProjectSelection();
				m_openedProject = nullptr;
			}
			else
				isExitRequested = true;
		}
	}
	m_worldEditor->ClearScene();

	SE::SaveProjects(imguiEditorPass->m_ProjectSelector.m_projectsList);
}

void EditorApp::InitResourceLoaders(ID3D11Device* device)
{
	ResourceManagerFacade::Instance().Initialize(256 * 1024 * 1024);

	auto texLoader = eastl::make_unique<TextureLoader>(device);
	ResourceLoaderFactory::RegisterLoader(SunshineResource::ResourceType::TEXTURE,
		eastl::move(texLoader));

	AssetPath ap(
		SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnhandledTextureColor),
		AssetPath::AssetSource::Engine);
	ResourceManagerFacade::Instance().LoadByPath(ap);

	ap = AssetPath(
		SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
		AssetPath::AssetSource::Engine);
	ResourceManagerFacade::Instance().LoadByPath(ap);
}

void EditorApp::UpdateGame(float deltaTime)
{
	if (m_gamePaused)
		return;
	//m_currentGame->m_playerObject->m_playerController.m_inputManager.Update();
	m_currentGame->Update(deltaTime);
}

void EditorApp::UpdateEditor(float deltaTime) 
{
	if (m_audioEditor) {
		m_audioEditor->Update();
	}

	if (!imguiEditorPass->IsFocusedGameViewport)
	{
		// Reset input state when not focused
		m_worldEditor->m_editorInputManager.Reset();
		m_worldEditor->IsRightMousePressed = false;
	}
	else {
		// Update input manager for edge detection
		m_worldEditor->m_editorInputManager.Update();
	}
	m_worldEditor->Update(deltaTime);
}

void EditorApp::Render() {
	SUNSHINE_ROOT_DIR;
	// Passes
	m_renderingSystem->Render();

	m_renderingSystem->PresentFrame();
}

void EditorApp::OnResize(UINT resizeWidth, UINT resizeHeight)
{
	if (m_initialized)
	{
		m_renderingSystem->PreResize();
		imguiEditorPass->PreResize();

		m_renderingSystem->OnResize(resizeWidth, resizeHeight);
		imguiEditorPass->OnResize(resizeWidth, resizeHeight, m_renderingSystem->GetBackBuffer());
	}
}

void EditorApp::SetIcon(HWND hwnd)
{
	// Load a .ico file from your executable directory
	HICON hIconLarge = (HICON)LoadImage(
		NULL,                 // No instance handle (load from file)
		// Path to .ico file
		JoinWchar_Wstring(EDITOR_ASSETS_DIR, L"Icons/SunshineLogo_transparent_256.ico").c_str(),
		IMAGE_ICON,           // Load an icon
		256, 256,               // Desired icon size (large)
		LR_LOADFROMFILE       // Load from file
	);

	HICON hIconSmall = (HICON)LoadImage(
		NULL,
		// Path to .ico file
		JoinWchar_Wstring(EDITOR_ASSETS_DIR, L"Icons/SunshineLogo_transparent_32.ico").c_str(),
		IMAGE_ICON,
		32, 32,
		LR_LOADFROMFILE
	);

	// Assign to window
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconLarge);
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
}

void EditorApp::HandleKeyDown(Keys key) 
{
	if (!imguiEditorPass->IsFocusedGameViewport)
		return;

	if (m_runtimeMode == RuntimeMode::WORLD_EDITOR_MODE)
	{
		m_worldEditor->HandleKeyDown(key);
	}
	else if(m_runtimeMode == RuntimeMode::GAME_MODE)
	{
		m_currentGame->HandleKeyDown(key);
	}
}

void EditorApp::HandleKeyUp(Keys key)
{
	if (!imguiEditorPass->IsFocusedGameViewport)
		return;

	if (m_runtimeMode == RuntimeMode::WORLD_EDITOR_MODE)
	{
		m_worldEditor->HandleKeyUp(key);
	}
	else if (m_runtimeMode == RuntimeMode::GAME_MODE)
	{
		m_currentGame->HandleKeyUp(key);
	}
}

void EditorApp::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	if (!imguiEditorPass->IsFocusedGameViewport)
		return;

	if (m_runtimeMode == RuntimeMode::WORLD_EDITOR_MODE)
	{
		m_worldEditor->HandleMouseMove(args);
	}

	if (m_runtimeMode == RuntimeMode::GAME_MODE)
	{
		m_currentGame->HandleMouseMove(args);
	}
}

void EditorApp::RunGame()
{
	m_runtimeMode = RuntimeMode::GAME_MODE;

	if (m_loadedSceneType == SE::SceneType::Custom && m_openedProject)
	{
		m_openedProject->Save();
	}
	m_worldEditor->m_particleSystem->DisableAllEmitters();
	m_worldEditor->Pause();

	// Init WorldEditor with all it's passes
	m_currentGame = eastl::make_unique<Game>();
	m_currentGame->SetupRendering(m_renderingSystem,
		m_worldEditor->m_screenWidth, m_worldEditor->m_screenHeight);
	// m_currentGame->SetParticleSystem(m_worldEditor->m_renderer->m_particleSystem);
	
	m_currentGame->m_particleSystem->EnableAllEmitters();

	if (m_audioEditor) {
		m_audioEditor->SetAudioSystem(&AudioSystem::Get());
	}

	if (AudioSystem::IsInitialized()) {
		AudioSystem::Get().StopAll();
	}
	
	if (m_loadedSceneType == SE::SceneType::Custom && m_openedProject)
	{
		eastl::wstring scenePath = m_openedProject->GetScenePath();
		m_currentGame->LoadScene(scenePath.c_str());
	}
	else if (m_loadedSceneType == SE::SceneType::Default)
	{
		m_currentGame->LoadDefaultScene();
	}
	else if (m_loadedSceneType == SE::SceneType::GAI)
	{
		m_currentGame->LoadGAIScene();
	}
	else if (m_loadedSceneType == SE::SceneType::Parent)
	{
		m_currentGame->LoadParentScene();
	}
	else if (m_loadedSceneType == SE::SceneType::Lua)
	{
		m_currentGame->LoadLuaScene();
	}
	else if (m_loadedSceneType == SE::SceneType::Resources)
	{
		m_currentGame->LoadResourcesScene();
	}

	m_renderingSystem->AddRenderGroup(m_currentGame->m_renderer.get());
	
	imguiEditorPass->SetVieportGBuffer(
		m_currentGame->m_renderer->m_GBuffer.get());

	m_deltaTime = 0.0f;

	accumulator = 0.0f;
	accumulatorLimit = 4.0f * physicsUpdateMs;

	// FPS statitistic
	frameCount = 0;
	FPSstatisticTimer = 0;

	m_currentGame->m_particleSystem->Enable();
	m_currentGame->m_particleSystem->EnableAllEmitters();

	m_currentGame->m_timer.Reset();
}

void EditorApp::PauseGame() {
	m_gamePaused = true;
}

void EditorApp::ContinueGame() {
	m_gamePaused = false;

	m_currentGame->m_timer.Reset();
}

void EditorApp::StopGame() {
	m_currentGame->Stop();
	m_currentGame->m_particleSystem->DisableAllEmitters();
	m_currentGame->m_particleSystem;
	m_worldEditor->OnResize(m_currentGame->m_screenWidth, m_currentGame->m_screenHeight);
	if (m_audioEditor) {
		AudioSystem::Get().LoadFromJson(
			m_audioEditor->GetConfigPath()
		);
	}
	m_currentGame->ClearScene();
	m_currentGame.reset(NULL);
	m_currentGame->ClearScene();
	m_currentGame.reset(NULL);
	m_renderingSystem->RemoveRenderGroup("GameDeferred");

	m_runtimeMode = RuntimeMode::WORLD_EDITOR_MODE;
	imguiEditorPass->SetVieportGBuffer(
		m_worldEditor->m_renderer->m_GBuffer.get());

	m_worldEditor->m_renderer->m_particleSystem->SetRenderer(m_worldEditor->m_renderer.get());
	m_worldEditor->m_lightPass->m_particleSystem = m_worldEditor->m_renderer->m_particleSystem.get();
	m_worldEditor->Start();
	// There should be loading scene to world editor (deserializing)
	// m_currentGame->UnloadScene(...);
	// m_worldEditor->LoadScene(...);

	m_deltaTime = 0.0f;

	accumulator = 0.0f;
	accumulatorLimit = 4.0f * physicsUpdateMs;

	// FPS statitistic
	frameCount = 0;
	FPSstatisticTimer = 0;

	m_worldEditor->m_timer.Reset();
}

void EditorApp::SaveProject()
{
	if (m_openedProject && m_loadedSceneType == SE::SceneType::Custom)
	{
		eastl::string error = m_openedProject->Save();
		if (!error.empty())
			return;

		SE::SaveProjects(imguiEditorPass->m_ProjectSelector.m_projectsList);
		imguiEditorPass->m_ProjectSelector.RefreshProjectList();
	}
}

bool EditorApp::OpenProject()
{
	m_openedProject = imguiEditorPass->GetSelectedProject();
	m_loadedSceneType = imguiEditorPass->m_ProjectSelector.GetSelectedSceneType();
	if (m_loadedSceneType == SE::SceneType::Custom)
	{
		eastl::string error = m_openedProject->Open();
		if (!error.empty())
			return false;
	}
	else
	{
		switch (m_loadedSceneType)
		{
		case SE::SceneType::GAI:
			m_worldEditor->CreateGAIScene();
			break;
		case SE::SceneType::Default:
			m_worldEditor->CreateDefaultScene();
			break;
		case SE::SceneType::Parent:
			m_worldEditor->CreateParentScene();
			break;
		case SE::SceneType::Resources:
			m_worldEditor->CreateResourcesScene();
			break;
		}
	}
	
	SetupAssetsDirectory();
	return true;
}

void EditorApp::CloseProject()
{
	m_projectSelected = NULL;

	m_renderingSystem->RemoveRenderGroup("PlayerViewport");
	// m_openedProject = NULL;
	// m_loadedSceneType = SE::SceneType::Custom;
}

void EditorApp::SetupAssetsDirectory()
{
	switch (m_loadedSceneType)
	{
	    case SE::SceneType::Custom:
	        if (m_openedProject)
	        {
	            ContentBrowserPanel::s_AssetsDirectory = 
	                std::filesystem::path(m_openedProject->GetFullPath().c_str());
	        }
	        break;
	    case SE::SceneType::GAI:
	        ContentBrowserPanel::s_AssetsDirectory = 
	            std::filesystem::path(SE::Project(L"GAI/").GetFullPath().c_str());
	        break;
	    case SE::SceneType::Default:
	        ContentBrowserPanel::s_AssetsDirectory = 
	            std::filesystem::path(SE::Project(L"DefaultScene/").GetFullPath().c_str());
	        break;
	    case SE::SceneType::Parent:
	        ContentBrowserPanel::s_AssetsDirectory = 
	            std::filesystem::path(SE::Project(L"Hierarchy/").GetFullPath().c_str());
	        break;
	    case SE::SceneType::Lua:
	        ContentBrowserPanel::s_AssetsDirectory = 
	            std::filesystem::path(SE::Project(L"Lua/").GetFullPath().c_str());
	        break;
	    case SE::SceneType::Resources:
	        ContentBrowserPanel::s_AssetsDirectory = 
	            std::filesystem::path(SE::Project(L"Resources/").GetFullPath().c_str());
	        break;
	    default:
	        ContentBrowserPanel::s_AssetsDirectory = 
	            std::filesystem::path(SE::Project(L"DefaultScene/").GetFullPath().c_str());
	        break;
	}
}
