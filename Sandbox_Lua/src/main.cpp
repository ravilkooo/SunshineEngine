#include "Game.h"
#include <windows.h>
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <Jolt/Jolt.h>
#include <VGJS.h>
#include <assimp/Importer.hpp>
#include <EASTL/allocator.h>
#include <vector>
#include <string>
#include <array>
#include "fmod.hpp"
#include "common.h"
#include <filesystem>
#include "LuaLogic.h"
#include <TestGameObjects.h>
#include <Windows/WndProc.h>

namespace fs = std::filesystem;

bool showExampleWindow = true;

float currentFloatValue = 0.0f;
float lastFloatValue = 0.0f;

int imgui_test();
int fmod_test();
int lua_test();
int game_test();

LuaLogic luaLogic;
//TestGameObjects testGameObjects;

int main() {
	luaLogic.Init();

	imgui_test();
	//testGameObjects.Run();

	luaLogic.Cleanup();
	return 0;
}

int fmod_test() {
	FMOD_Main();

	return 0;
}

int imgui_test()
{
	// 1. Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc::WndProcImGui, 0, 0,
					  GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
					  L"ImGuiTestWindow", NULL };
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(wc.lpszClassName, L"Lua Test", WS_OVERLAPPEDWINDOW,
		100, 100, 800, 600, NULL, NULL, wc.hInstance, NULL);

	// 2. Initialize DirectX 11
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	DXGI_SWAP_CHAIN_DESC scDesc = {};
	scDesc.BufferCount = 1;
	scDesc.BufferDesc.Width = 800;
	scDesc.BufferDesc.Height = 600;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.OutputWindow = hwnd;
	scDesc.SampleDesc.Count = 1;
	scDesc.Windowed = TRUE;
	IDXGISwapChain* swapChain = nullptr;

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		0, nullptr, 0, D3D11_SDK_VERSION,
		&scDesc, &swapChain, &device, &featureLevel, &context);

	// 3. Create render target
	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11Texture2D* backBuffer = nullptr;
	swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	device->CreateRenderTargetView(backBuffer, nullptr, &rtv);
	backBuffer->Release();

	// 4. Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);

	// Show window
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	// 5. Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Text("Lua Script:");
		if (ImGui::BeginCombo("##LuaFile", luaLogic.luaFiles.empty() ? "" : luaLogic.luaFiles[luaLogic.selectedLuaFile].c_str())) {
			for (int i = 0; i < luaLogic.luaFiles.size(); ++i) {
				bool is_selected = (i == luaLogic.selectedLuaFile);
				if (ImGui::Selectable(luaLogic.luaFiles[i].c_str(), is_selected))
					luaLogic.selectedLuaFile = i;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Load Script")) {
			luaLogic.LoadScript();
		}

		if (luaLogic.scriptLoaded)
		{
			ImGui::Text("Function Name:"); ImGui::SameLine();
			ImGui::InputText("##FunctionName", luaLogic.functionName, IM_ARRAYSIZE(luaLogic.functionName));
			ImGui::SameLine();
			if (ImGui::Button("Find")) {
				luaLogic.FindFunction();
			}

			if (luaLogic.foundFunction) {
				ImGui::Text("Parameters:");
				for (int i = 0; i < luaLogic.params.size(); ++i) {
					ImGui::Text("%s (%s) =", luaLogic.params[i].name.c_str(), luaLogic.params[i].type.c_str());
					ImGui::SameLine();
					ImGui::InputText(("##p" + std::to_string(i)).c_str(), luaLogic.params[i].value.data(), luaLogic.params[i].value.size());
				}

				if (ImGui::Button("Call")) {
					luaLogic.CallFunction();
				}
				if (!luaLogic.lastResult.empty()) {
					ImGui::Text("%s", luaLogic.lastResult.c_str());
				}

			}
			else if (!luaLogic.errorMessage.empty()) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", luaLogic.errorMessage.c_str());
			}
		}



		if (ImGui::Button("Close")) {
			showExampleWindow = false;
			PostQuitMessage(0);
		}


		// Rendering
		ImGui::Render();
		context->OMSetRenderTargets(1, &rtv, nullptr);
		const float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
		context->ClearRenderTargetView(rtv, clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swapChain->Present(1, 0);
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	rtv->Release();
	swapChain->Release();
	context->Release();
	device->Release();

	return 0;
}


