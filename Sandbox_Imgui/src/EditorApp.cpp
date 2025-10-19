#include "EditorApp.h"

EditorApp::EditorApp() {

}

void EditorApp::InitEditorApp(UINT winWidth, UINT winHeight)
{
	m_applicationName = L"SunshineEngine";
	m_hInstance = GetModuleHandle(nullptr);

	m_winWidth = winWidth;
	m_winHeight = winHeight;

	m_timer = GameTimer();

	m_displayWindow = DisplayWindow(this, m_applicationName, m_hInstance,
		m_winWidth, m_winHeight, DisplayWindow::WndProcImGui);

	m_renderer = eastl::make_shared<DeferredRenderer>(
		m_displayWindow.m_hWnd,
		m_winWidth, m_winHeight);
	
	UINT worldEditorWidth = winWidth / 2;
	UINT worldEditorHeight = winHeight / 2;
	m_renderer->InitGBuffer(worldEditorWidth, worldEditorHeight);
	//m_renderer->InitGBuffer(m_winWidth, m_winHeight);

	// Init WorldEditor with all it's passes
	m_worldEditor = eastl::make_unique<WorldEditor>();
	m_worldEditor->InitWorldEditor(m_renderer, worldEditorWidth, worldEditorHeight);
	//m_worldEditor->InitWorldEditor(m_renderer, m_winWidth, m_winHeight);

	// Init lua/sol2 state
	m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::table, sol::lib::math);
	sol_ImGui::Init(m_lua);
	
	// Init imgui staff
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// enable docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(m_displayWindow.m_hWnd);
	ImGui_ImplDX11_Init(
		m_renderer->GetDevice(),
		m_renderer->GetDeviceContext());

	// Imgui Pass
	ImguiEditorPass* imguiEditorPass = new ImguiEditorPass(
		m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
		m_renderer->GetBackBuffer(),
		m_winWidth,
		m_winHeight,
		m_renderer->pGBuffer
	);
	m_renderer->AddPass(imguiEditorPass);

	// Show window
	// ShowWindow(m_displayWindow.m_hWnd, SW_SHOWDEFAULT);
	// UpdateWindow(m_displayWindow.m_hWnd);

}
EditorApp::~EditorApp() {
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void EditorApp::Run()
{
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
}

void EditorApp::Update(float deltaTime) {
	m_worldEditor->Update(deltaTime);
}

void EditorApp::Render() {

	// Passes
	m_renderer->RenderScene(m_worldEditor->m_scene);
	m_renderer->PresentFrame();
}