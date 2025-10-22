#pragma once
#include <wtypes.h>
#include <windows.h>
#include <d3d11.h>
#include "../../../ThirdParty/imgui/imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace WndProc
{
	// Forward declare helper functions
	LRESULT CALLBACK WndProcImGui(HWND, UINT, WPARAM, LPARAM);
}
