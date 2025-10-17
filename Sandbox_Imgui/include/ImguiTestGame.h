#pragma once

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <Game.h>

class ImguiTestGame : public Game
{
public:
	bool is_layout_initialized = false;

	~ImguiTestGame() {
		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	sol::state lua;
	void Update(float deltaTime) override;
	void Render() override;
	void InitGame();
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Forward declare helper functions
LRESULT CALLBACK WndProcImGui(HWND, UINT, WPARAM, LPARAM);
