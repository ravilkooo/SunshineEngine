#ifndef DISPLAY_WIN_H
#define DISPLAY_WIN_H

// SunshineLibs

#include "SunshineEngineAPI.h"
#include "InputDevice.h"

#include <windows.h>
#include <iostream>

// ThirdPartyLibs

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>


class SUNSHINE_ENGINE_API DisplayWindow
{
	friend class WindowsApp;

public:
	static UINT g_resizeWidth;
	static UINT g_resizeHeight;

	static WindowsApp* mApp;
	static WindowsApp* GetApp();

	HWND m_hWnd;

	DisplayWindow();
	DisplayWindow(WindowsApp* winApp, LPCWSTR applicationName,
		HINSTANCE hInstance, UINT screenWidth, UINT screenHeight,
		WNDPROC lpfnWndProc = WndProc);

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
	// Imgui proc (temporaly?)
	static LRESULT CALLBACK WndProcImGui(HWND, UINT, WPARAM, LPARAM);
	
	// My custom method
	// static LRESULT CALLBACK WndProc_RawInput(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

	// My custom method
	// static void HandleRawInput(HRAWINPUT hRawInput, InputHandler* inputHandler);

	// My custom method
	// void RegisterRawInput(HWND hWnd);
	
	// My custom method
	// InputHandler* GetInputHandler() { return inputHandler; }

private:
	// My custom
	// InputHandler* inputHandler; // Не статический член класса

	// void Initialize(LPCWSTR applicationName, HINSTANCE hInstance, int screenWidth, int screenHeight);
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // DISPLAY_WIN_H