#pragma once
#include <imgui.h>

#include <Utils/ILogManager.h>

class LogPanel
{
public:
    LogPanel(const char* title, ILogManager::LogTarget target);

    void OnImguiRender(bool& showLogPanel);
    void SetBottomOffset(float bottomHeight);
private:
    ImGuiTextFilter       m_Filter;
    bool                  m_AutoScroll = true;
    float                 m_BottomOffset = 30.0f;
    float                 m_CurrentHeight = 100.0f;
    const char*           m_Title;
    ILogManager::LogTarget m_Target;
};


