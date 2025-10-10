#include "DeferredGame.h"
#include <Jolt/Jolt.h>
#include <VGJS.h>
#include <assimp/Importer.hpp>
#include <EASTL/allocator.h>
#include <imgui.h>
#include <lua.hpp>
#include <vector>
#include <string>
#include <array>
#include "fmod.hpp"
#include "common.h"
#include <filesystem>
#include "LuaLogic.h"

namespace fs = std::filesystem;

std::vector<ParamEntry> params;
std::string lastResult;
std::string errorMessage;
std::vector<std::string> luaFiles;
std::string assetsPath;
std::string scriptPath;
lua_State* L = nullptr;

static int selectedLuaFile = 0;
bool showExampleWindow = true;
bool scriptLoaded = false;
bool foundFunction = false;

char functionName[128] = "";
float currentFloatValue = 0.0f;
float lastFloatValue = 0.0f;

int imgui_test();
void CallLuaFunction();
int fmod_test();
int lua_test();
void Init_Lua_File();
void Lua_Init_Environment();
void LoadLuaScript();
void Lua_Cleanup();
void FindLuaFunction();
void CallLuaFunction();
int game_test();

#pragma region Utils
std::string wstringToString(const std::wstring& wideStr)
{
	return std::string(wideStr.begin(), wideStr.end());
}

void ScanLuaFiles(const std::string& dirPath) {
	luaFiles.clear();
	for (auto& entry : fs::directory_iterator(dirPath)) {
		if (entry.is_regular_file()) {
			std::string filename = entry.path().filename().string();
			if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".lua")
				luaFiles.push_back(filename);
		}
	}
}
#pragma endregion

LuaLogic luaLogic;
#pragma region LuaLogic
void Lua_Cleanup() {
	if (L) {
		lua_close(L);
		L = nullptr;
	}
	scriptLoaded = false;
}

void Lua_Init_Environment() {
	L = luaL_newstate();

	// open standard libraries
	luaL_openlibs(L);

	if (luaL_dofile(L, scriptPath.c_str()) != LUA_OK) {
		std::cerr << "Error running Lua script: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
}

void Init_Lua_File()
{
	assetsPath = wstringToString(SANDBOX_LUA_ASSETS_DIR) + "Scripts";
	ScanLuaFiles(assetsPath);
	if (!luaFiles.empty()) {
		selectedLuaFile = 0;
		scriptPath = assetsPath + "/" + luaFiles[0];
	}
	std::cout << scriptPath << "\n";
}

void LoadLuaScript() {
	scriptPath = assetsPath + "/" + luaFiles[selectedLuaFile];
	Lua_Cleanup();
	Lua_Init_Environment();
	errorMessage.clear();
	foundFunction = false;
	params.clear();
	lastResult.clear();
}

void FindLuaFunction() {
	foundFunction = false;
	errorMessage.clear();
	params.clear();

	if (L && strlen(functionName) > 0) {
		lua_getglobal(L, functionName);
		if (lua_isfunction(L, -1)) {
			foundFunction = true;
			lua_pop(L, 1);

			lua_getglobal(L, (std::string(functionName) + "_params").c_str());
			if (lua_istable(L, -1)) {
				int n = lua_rawlen(L, -1);
				for (int i = 1; i <= n; ++i) {
					lua_rawgeti(L, -1, i);
					ParamEntry entry;
					lua_getfield(L, -1, "name");
					entry.name = lua_tostring(L, -1);
					lua_pop(L, 1);
					lua_getfield(L, -1, "type");
					entry.type = lua_tostring(L, -1);
					lua_pop(L, 1);
					params.push_back(entry);
					lua_pop(L, 1); // entry
				}
			}
			lua_pop(L, 1); // params table
		}
		else {
			lua_pop(L, 1);
			errorMessage = "No such function: " + std::string(functionName);
		}
	}
	else {
		errorMessage = "Lua not initialized or function name empty!";
	}
}
void CallLuaFunction()
{
	lua_getglobal(L, functionName);
	for (const auto& p : params) {
		if (p.type == "number") lua_pushnumber(L, atof(p.value.data()));
		else if (p.type == "bool") lua_pushboolean(L, std::string(p.value.data()) == "true" || std::string(p.value.data()) == "1");
		else lua_pushstring(L, p.value.data());
	}
	int argCount = params.size();
	if (lua_pcall(L, argCount, 1, 0) != LUA_OK) {
		errorMessage = "Lua error: " + std::string(lua_tostring(L, -1));
		lua_pop(L, 1);
		lastResult = "";
	}
	else {
		char buf[256];
		if (lua_isnumber(L, -1))
			snprintf(buf, sizeof(buf), "Result: %f", lua_tonumber(L, -1));
		else if (lua_isstring(L, -1))
			snprintf(buf, sizeof(buf), "Result: %s", lua_tostring(L, -1));
		else if (lua_isboolean(L, -1))
			snprintf(buf, sizeof(buf), "Result: %s", lua_toboolean(L, -1) ? "true" : "false");
		else
			snprintf(buf, sizeof(buf), "Result: <unknown type>");
		lastResult = buf;
		lua_pop(L, 1);
	}
}
#pragma endregion

int main() {
	Init_Lua_File();
	Lua_Init_Environment();

	game_test();

	Lua_Cleanup();
	return 0;
}

int game_test() {
	imgui_test();
	DeferredGame game = DeferredGame();
	game.Run();

	return 0;
}

int fmod_test() {
	FMOD_Main();

	return 0;
}

int imgui_test()
{
	// 1. Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProcImGui, 0, 0,
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
		if (ImGui::BeginCombo("##LuaFile", luaFiles.empty() ? "" : luaFiles[selectedLuaFile].c_str())) {
			for (int i = 0; i < luaFiles.size(); ++i) {
				bool is_selected = (i == selectedLuaFile);
				if (ImGui::Selectable(luaFiles[i].c_str(), is_selected))
					selectedLuaFile = i;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Load Script")) {
			LoadLuaScript();
			scriptLoaded = (L != nullptr);
		}

		if (scriptLoaded)
		{
			ImGui::Text("Function Name:"); ImGui::SameLine();
			ImGui::InputText("##FunctionName", functionName, IM_ARRAYSIZE(functionName));
			ImGui::SameLine();
			if (ImGui::Button("Find")) {
				FindLuaFunction();
			}

			if (foundFunction) {
				ImGui::Text("Parameters:");
				for (int i = 0; i < params.size(); ++i) {
					ImGui::Text("%s (%s) =", params[i].name.c_str(), params[i].type.c_str());
					ImGui::SameLine();
					ImGui::InputText(("##p" + std::to_string(i)).c_str(), params[i].value.data(), params[i].value.size());
				}

				if (ImGui::Button("Call")) {
					CallLuaFunction();
				}
				if (!lastResult.empty()) {
					ImGui::Text("%s", lastResult.c_str());
				}

			}
			else if (!errorMessage.empty()) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", errorMessage.c_str());
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


