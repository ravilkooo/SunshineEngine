#include "UI/BottomBarPanel.h"

void BottomBarPanel::OnImGuiRender(bool* showEditorLog, bool* showGameLog)
{
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - m_Height));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, m_Height));
    
    ImGui::Begin("Bottom Bar", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    float buttonHeight = ImGui::GetFrameHeight();
    float windowHeight = ImGui::GetWindowHeight();
    ImGui::SetCursorPosY((windowHeight - buttonHeight) / 2.0f);

    if (ImGui::Button("Editor Log"))
        if (*showEditorLog)
            *showEditorLog = false;
        else
        {
            *showEditorLog = true;
            *showGameLog = false; 
        }

    ImGui::SameLine();

    if (ImGui::Button("Game Log"))
        if (*showGameLog)
            *showGameLog = false;
        else
        {
            *showGameLog = true;
            *showEditorLog = false; 
        }

    ImGui::End();
}

float BottomBarPanel::GetHeight()
{
    return m_Height;
}

void BottomBarPanel::SetHeight(float bottomBarHeight)
{
    m_Height = bottomBarHeight;
}