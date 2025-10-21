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
	m_worldEditor = eastl::make_shared<WorldEditor>();
	m_worldEditor->InitWorldEditor(m_renderer, worldEditorWidth, worldEditorHeight);
	//m_worldEditor->InitWorldEditor(m_renderer, m_winWidth, m_winHeight);

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
		m_renderer->GetDevice(),
		m_renderer->GetDeviceContext());

	// Imgui Pass
	imguiEditorPass = eastl::make_shared<ImguiEditorPass>(
		m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
		m_renderer->GetBackBuffer(),
		m_winWidth,
		m_winHeight,
		m_renderer->pGBuffer,
		m_worldEditor
	);
	m_renderer->AddPass(imguiEditorPass);

	m_initialized = true;

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

void EditorApp::OnResize(UINT resizeWidth, UINT resizeHeight)
{
	if (m_initialized)
	{
		m_renderer->PreResize();
		imguiEditorPass->PreResize();
		m_renderer->OnResize(resizeWidth, resizeHeight);
		imguiEditorPass->OnResize(resizeWidth, resizeHeight, m_renderer->GetBackBuffer());
	}
}

void EditorApp::SetIcon(HWND hwnd)
{
	// Load a .ico file from your executable directory
	HICON hIconLarge = (HICON)LoadImage(
		NULL,                 // No instance handle (load from file)
		// Path to .ico file
		JoinWchar_Wchar(EDITOR_ASSETS_DIR, L"Icons/SunshineLogo_transparent_256.ico"),
		IMAGE_ICON,           // Load an icon
		256, 256,               // Desired icon size (large)
		LR_LOADFROMFILE       // Load from file
	);

	HICON hIconSmall = (HICON)LoadImage(
		NULL,
		// Path to .ico file
		JoinWchar_Wchar(EDITOR_ASSETS_DIR, L"Icons/SunshineLogo_transparent_32.ico"),
		IMAGE_ICON,
		32, 32,
		LR_LOADFROMFILE
	);

	// Assign to window
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconLarge);
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
}
