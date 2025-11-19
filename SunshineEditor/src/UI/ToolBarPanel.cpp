#include "UI/ToolBarPanel.h"

#include <iostream>
#include <ostream>

#include <EditorApp.h>

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

    if (!isPlaying)
    {
        if (ImGui::Button("Play"))
        {
            isPlaying = true; 
            std::cout << "Play\n";
            if (m_editorApp)
                m_editorApp->LaunchGame();
        }
        ImGui::SameLine();
        if (ImGui::Button("Save"))
        {
            if (m_editorApp) {
                // To-do: there should be path to opened project
                // to-do: class Project
                m_editorApp->m_worldEditor->SaveScene(WcharToChar(JoinWchar_Wchar(PROJECTS_DIR, L"DefaultScene/scene.json")));
            }
        }
    }
    else
    {
        if (ImGui::Button("Pause"))
        {
            std::cout << "Pause\n";
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            isPlaying = false; 
            std::cout << "Stop\n";
            if (m_editorApp)
                m_editorApp->StopGame();
        }
    }

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

void ToolbarPanel::SetEditorApp(EditorApp* editorApp)
{
    m_editorApp = editorApp;
}
