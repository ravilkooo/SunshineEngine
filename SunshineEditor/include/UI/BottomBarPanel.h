#pragma once
#include <imgui.h>

class BottomBarPanel
{
public:
    void OnImGuiRender(bool* showEditorLog, bool* showGameLog);
    float GetHeight();
    void SetHeight(float bottomBarHeight);

private:
    float m_Height = 30.0f;
};
