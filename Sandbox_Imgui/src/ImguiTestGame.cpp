#include "ImguiTestGame.h"

void ImguiTestGame::InitGame() {

	m_applicationName = L"Game";
	m_hInstance = GetModuleHandle(nullptr);

	m_winWidth = 1200;
	m_winHeight = 800;

	m_timer = GameTimer();

	m_scene = Scene();

	m_displayWindow = DisplayWindow(this, m_applicationName, m_hInstance,
		m_winWidth, m_winHeight, DisplayWindow::WndProcImGui);

	m_renderer = eastl::make_unique<DeferredRenderer>(
		m_displayWindow.m_hWnd,
		m_winWidth, m_winHeight);

	UINT worldEditorWidth = 600;
	UINT worldEditorHeight = 400;
	m_renderer->InitGBuffer(worldEditorWidth, worldEditorHeight);

	// GPass
	{
		GPass* gPass = new GPass(
			m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->GetBackBuffer(),
			m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(gPass);
	}
	{
		LightPass* lightPass = new LightPass(
			m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->GetBackBuffer(),
			m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(lightPass);
	}
	// FinalPass
	{
		FinalPass* colorPass = new FinalPass(
			m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->GetBackBuffer(),
			m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(colorPass);
	}

	auto factory = GameObjectFactory();

	m_scene.AddGameObject(eastl::move(factory.CreateFinalPassQuad(m_renderer->GetDevice())));


	m_scene.AddGameObject(eastl::move(factory.CreateDirectionalLightObject(m_renderer->GetDevice(), m_renderer->GetMainCamera(),
		{
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3::Zero, 0,
			DXSM::Vector3(1.0f, -1.0f, 1.0f), 0
		}
	)));
	m_scene.AddGameObject(eastl::move(factory.CreateAmbientLightObject(m_renderer->GetDevice(), m_renderer->GetMainCamera(),
		{ DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f) * 0.5, 1.0f })));

	m_scene.AddGameObject(eastl::move(factory.CreateSkyBox(
		m_renderer->GetDevice(), m_renderer->GetMainCamera(), { DXSM::Vector3::One, 0.0f }, L"Default")));

	m_scene.AddGameObject(eastl::move(factory.CreateDefaultBoxObject(m_renderer->GetDevice(), 2.0f, 2.0f)));

	resultSRV = m_renderer->pGBuffer->pLightSRV.Get();
	// gPass->pGBuffer->pLightSRV.Get();
	// renderer->GetBackBuffer();
}

void ImguiTestGame::Update(float deltaTime) {
	m_scene.gameObjects[4]->GetComponent<TransformComponent>()->m_localRotation.y += deltaTime;
}

// Win32 message handler
LRESULT CALLBACK WndProcImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) {}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DisplayWindow::WndProc(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ImguiTestGame::Render() {

	// Game::Render();	
	// Passes
	for (int i = 0; i < m_renderer->passes.size() - 1; i++) {
		m_renderer->GetDeviceContext()->ClearState();
		RenderPass* pass = m_renderer->passes[i];
		pass->StartFrame();
		pass->Pass(m_scene);
		pass->EndFrame();
	}

	m_renderer->GetDeviceContext()->ClearState();

	RenderPass* pass = m_renderer->passes.back();
	pass->StartFrame();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Создаём DockSpace поверх главного вьюпорта
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	// ----- Docking -------
	//ImGui::DockSpaceOverViewport(0u, ImGui::GetMainViewport());

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("DockSpace Demo", nullptr, window_flags);
	ImGui::PopStyleVar(2);

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	if (!is_layout_initialized)
	{
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

		ImGuiID dock_main_id = dockspace_id;
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

		ImGui::DockBuilderDockWindow("Main Game Viewport", dock_main_id);
		ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_id_left);
		ImGui::DockBuilderDockWindow("Properties", dock_id_right);
		ImGui::DockBuilderDockWindow("Content Browser", dock_id_down);

		ImGui::DockBuilderFinish(dockspace_id);

		is_layout_initialized = true;
	}
	ImGui::End();

	ImGui::Begin("Scene Hierarchy");
	ShowSceneHierarchy();  // Реализация вашего иерархического списка сцен и объектов
	ImGui::End();

	ImGui::Begin("Content Browser");
	ShowContentBrowser();  // Ваш браузер ассетов
	ImGui::End();

	// Можно также добавить окно снизу для логов, свойств и прочего
	ImGui::Begin("Properties");
	ShowProperties();
	ImGui::End();

	// Главное окно игрового мира
	ImGui::Begin("Main Game Viewport");
	RenderGameWorld();
	ImVec2 avail = ImGui::GetContentRegionAvail();
	ImGui::Image((ImTextureID)resultSRV.Get(), avail);
	ImGui::End();

	// -----------------

	// Запуск Lua-скрипта с вызовами ImGui

	/*lua.script(R"(
		if ImGui.Begin('Hello from Lua') then
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
			ImGui.Text('This is a text line rendered via Lua bindings')
			ImGui.End()
		  end
		)");*/

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	pass->EndFrame();
	m_renderer->PresentFrame();
}

void ImguiTestGame::RenderGameWorld()
{
	// Здесь нужно отобразить ваше игровое содержимое, пока заглушка
	ImGui::Text("Game World Render Here");
}

void ImguiTestGame::ShowSceneHierarchy()
{
	ImGui::Text("Scene Hierarchy");
	if (ImGui::TreeNode("Root"))
	{
		ImGui::BulletText("Object A");
		ImGui::BulletText("Object B");
		ImGui::TreePop();
	}
}

void ImguiTestGame::ShowContentBrowser()
{
	ImGui::Text("Content Browser");
	ImGui::Button("Import Asset");
}

void ImguiTestGame::ShowProperties()
{
	ImGui::Text("Properties");
	//ImGui::InputText("Name", nullptr, 0); // Пример
}
