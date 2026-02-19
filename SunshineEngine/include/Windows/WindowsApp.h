#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <Windows/DisplayWindow.h>

class WindowsApp {
public:
    DisplayWindow m_displayWindow;

    HINSTANCE m_hInstance;
    LPCWSTR m_applicationName;

    UINT m_winWidth = 800;
    UINT m_winHeight = 800;

    virtual void OnResize(UINT resizeWidth, UINT resizeHeight) = 0;
    virtual void SetIcon(HWND hwnd) = 0;
};