#pragma once
#include <imgui.h>

class MainMenuBarPanel
{
public:
    void OnImGuiRender(); 
    float GetHeight() const { return m_Height; }

private:
    float m_Height = 0.0f; 
};
