#ifndef DISPLAY_WIN_H
#define DISPLAY_WIN_H



#include <windows.h>
#include <iostream>
#include "InputDevice.h"

class DisplayWindow
{
	friend class Game;

public:
	UINT screenWidth = 800;
	UINT screenHeight = 800;
	HWND hWnd;

	DisplayWindow();
	DisplayWindow(Game* inGame, LPCWSTR applicationName, HINSTANCE hInstance, int screenWidth, int screenHeight);

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
	
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

#endif // DISPLAY_WIN_H