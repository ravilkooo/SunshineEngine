#include <iostream>
#include <sol_ImGui.h>
#include <sol/sol.hpp>
#include <imgui.h>

#include <ImguiTestGame.h>
#include <Graphics/DeferredRenderer.h>
#include <Graphics/GPass.h>
#include <Graphics/LightPass.h>
#include <Graphics/MainColorPass.h>


#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

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
	ImGui_ImplDX11_Init(game->renderer->GetDevice(), game->renderer->GetDeviceContext()); // device и device_context — ваши D3D11 объекты

	game->Run();
}

void ImguiTestGame::Update(float deltaTime) {

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
	GPass* gPass;
	{
		gPass = new GPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight);

		renderer->SetMainCamera(gPass->GetCamera());
		renderer->mainCamera->SetPosition({ 0, 0, -10 });

		renderer->AddPass(gPass);
	}
	{
		LightPass* lightPass;
		lightPass = new LightPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight, gPass->pGBuffer, gPass->GetCamera());


		lightPass->AddPerFrameBind(new Bind::Texture(renderer->GetDevice(), gPass->pGBuffer->pNormalSRV.Get(), 0u));
		lightPass->AddPerFrameBind(new Bind::Texture(renderer->GetDevice(), gPass->pGBuffer->pAlbedoSRV.Get(), 1u));
		lightPass->AddPerFrameBind(new Bind::Texture(renderer->GetDevice(), gPass->pGBuffer->pSpecularSRV.Get(), 2u));
		lightPass->AddPerFrameBind(new Bind::Texture(renderer->GetDevice(), gPass->pGBuffer->pWorldPosSRV.Get(), 3u));

		// Usual sampler for all SRV
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		lightPass->AddPerFrameBind(new Bind::Sampler(renderer->GetDevice(), samplerDesc, 0u));

		renderer->AddPass(lightPass);
	}
	// FinalPass
	{
		FinalPass* finalPass = new FinalPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight);

		finalPass->SetCamera(renderer->GetMainCamera());

		finalPass->AddPerFrameBind(new Bind::Texture(renderer->GetDevice(), gPass->pGBuffer->pLightSRV.Get(), 0u));

		// Usual sampler for all SRV
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		finalPass->AddPerFrameBind(new Bind::Sampler(renderer->GetDevice(), samplerDesc, 0u));

		renderer->AddPass(finalPass);
	}
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
	pass->Pass(scene);


	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	// ----- Docking -------
	ImGui::DockSpaceOverViewport(0u, ImGui::GetMainViewport());
	
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
	ImGui::Begin("Main Game Viewport", nullptr, window_flags);
	ImGui::End();

	ImGui::Begin("Scene Hierarchy");
	//ShowSceneHierarchy();  // Реализация вашего иерархического списка сцен и объектов
	ImGui::End();

	ImGui::Begin("Content Browser");
	//ShowContentBrowser();  // Ваш браузер ассетов
	ImGui::End();

	// Можно также добавить окно снизу для логов, свойств и прочего
	ImGui::Begin("Properties");
	//ShowProperties();
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
	renderer->PresentFrame();
}