#pragma once
#include <imgui.h>

class MainMenuBarPanel
{
public:
    void OnImGuiRender(); 
    float GetHeight() const { return m_MainMenuHeight; }

private:
    float m_MainMenuHeight = 0.0f; 
};
