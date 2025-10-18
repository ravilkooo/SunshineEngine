#include <iostream>

#include <sol/sol.hpp>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>

#include <sol_ImGui.h>

#include <ImguiTestGame.h>
#include <Graphics/DeferredRenderer.h>
#include <Graphics/GPass.h>
#include <Graphics/LightPass.h>
#include <Graphics/MainColorPass.h>

#include <GameObjectFactory.h>



int main() {
	std::cout << "Hello, friend!\n";

	/*
	Init game, device and windows inside game
	*/
	ImguiTestGame* game = new ImguiTestGame();
	game->InitGame();
	
	game->lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::table, sol::lib::math);

	sol_ImGui::Init(game->lua);

	// Init imgui staff
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// enable docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(game->displayWindow.hWnd);
	ImGui_ImplDX11_Init(game->renderer->GetDevice(), game->renderer->GetDeviceContext());

	game->Run();
}

void ImguiTestGame::InitGame() {

	applicationName = L"Game";
	hInstance = GetModuleHandle(nullptr);

	winWidth = 1000;
	winHeight = 800;

	timer = GameTimer();

	scene = Scene();

	displayWindow = DisplayWindow(this, applicationName, hInstance,
		winWidth, winHeight, WndProcImGui);

	renderer = new DeferredRenderer(displayWindow.hWnd, winWidth, winHeight);

	// GPass
	{
		GPass* gPass = new GPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight, renderer->pGBuffer, renderer->GetMainCamera());

		renderer->AddPass(gPass);
	}
	{
		LightPass* lightPass = new LightPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight, renderer->pGBuffer, renderer->GetMainCamera());

		renderer->AddPass(lightPass);
	}
	// FinalPass
	{
		FinalPass* colorPass = new FinalPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight, renderer->pGBuffer, renderer->GetMainCamera());

		renderer->AddPass(colorPass);
	}

	auto factory = GameObjectFactory();

	scene.AddGameObject(eastl::move(factory.CreateFinalPassQuad(renderer->GetDevice())));
	
	
	scene.AddGameObject(eastl::move(factory.CreateDirectionalLightObject(renderer->GetDevice(), renderer->GetMainCamera(),
		{
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3::Zero, 0,
			DXSM::Vector3(1.0f, -1.0f, 1.0f), 0
		}
		)));
	scene.AddGameObject(eastl::move(factory.CreateAmbientLightObject(renderer->GetDevice(), renderer->GetMainCamera(),
		{ DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f) * 0.5, 1.0f })));

	scene.AddGameObject(eastl::move(factory.CreateSkyBox(
		renderer->GetDevice(), renderer->GetMainCamera(), { DXSM::Vector3::One, 0.0f }, L"Default")));

	scene.AddGameObject(eastl::move(factory.CreateDefaultBoxObject(renderer->GetDevice(), 2.0f, 2.0f)));

	resultSRV = renderer->pGBuffer->pLightSRV.Get();
	// gPass->pGBuffer->pLightSRV.Get();
	// renderer->GetBackBuffer();
}

void ImguiTestGame::Update(float deltaTime) {
	scene.gameObjects[4]->GetComponent<TransformComponent>()->m_localRotation.y += deltaTime;
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
	for (int i = 0; i < renderer->passes.size() - 1; i++) {
		renderer->GetDeviceContext()->ClearState();
		RenderPass* pass = renderer->passes[i];
		pass->StartFrame();
		pass->Pass(scene);
		pass->EndFrame();
	}

	renderer->GetDeviceContext()->ClearState();
	
	 RenderPass* pass = renderer->passes.back();
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
	//RenderGameWorld();
	ImVec2 avail = ImGui::GetContentRegionAvail();
	ImGui::Image((ImTextureID) resultSRV.Get(), avail);
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

	// pass->Pass(scene);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	 pass->EndFrame();
	renderer->PresentFrame();
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
