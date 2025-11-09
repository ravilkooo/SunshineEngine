#pragma once
#include <imgui.h>
#include "LogManager.h"

class ToolbarPanel
{
public:
    void OnImGuiRender(float menuBarHeight);
    float GetHeight();
    void SetHeight(float toolbarHeight);

    bool isPlaying = false;
    
private:
    float m_Height = 25.0f;
};
