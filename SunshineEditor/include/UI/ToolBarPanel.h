#pragma once
#include <imgui.h>

class ToolbarPanel
{
public:
    void OnImGuiRender(float menuBarHeight);
    float getHeight();
    void setHeight(float toolbarHeight);
    
private:
    float m_ToolbarHeight = 25.0f;
};
