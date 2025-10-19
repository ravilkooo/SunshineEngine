#pragma once

#include <Windows/DisplayWindow.h>

class WindowsApp {
public:
    DisplayWindow m_displayWindow;

    HINSTANCE m_hInstance;
    LPCWSTR m_applicationName;

    UINT m_winWidth = 800;
    UINT m_winHeight = 800;
};