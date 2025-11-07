#include "UI/ToolBarPanel.h"

#include <iostream>
#include <ostream>

void ToolbarPanel::OnImGuiRender(float menuBarHeight)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, m_ToolbarHeight));
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));

    ImGui::Begin("Toolbar", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoBackground);

    float windowHeight = ImGui::GetWindowHeight();
    float buttonHeight = ImGui::GetFrameHeight(); 
    ImGui::SetCursorPosY((windowHeight - buttonHeight) / 2.0f);

    if (ImGui::Button("Play")) { std::cout << "Play\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Pause")) { std::cout << "Pause\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) { std::cout << "Stop\n"; }

    ImGui::End();
    ImGui::PopStyleVar(3);
}

float ToolbarPanel::getHeight()
{
    return m_ToolbarHeight;
}

void ToolbarPanel::setHeight(float toolbarHeight)
{
    m_ToolbarHeight = toolbarHeight;
}
