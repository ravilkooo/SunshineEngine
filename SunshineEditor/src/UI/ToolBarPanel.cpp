#include "UI/ToolBarPanel.h"
#include <iostream>
#include <ostream>
#include <Utils/StringUtils.h>
#include <EditorApp.h>

void ToolbarPanel::OnImGuiRender(float menuBarHeight)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, m_Height));
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
            LOG_GAME_INFO("Game started");
            if (m_editorApp)
                m_editorApp->LaunchGame();
        }
        ImGui::SameLine();
        if (ImGui::Button("Save"))
        {
            std::cout << "Saved scene\n";
            if (m_editorApp) {
                //m_editorApp->m_worldEditor->m_scene->ToJson();
                m_editorApp->SaveSceneToFile(WcharToChar(JoinWchar_Wchar(EDITOR_ASSETS_DIR, L"../scene.json")));
            }
        }
    }
    else
    {
        if (ImGui::Button("Pause"))
        {
            LOG_GAME_INFO("Game paused");
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            isPlaying = false; 
            std::cout << "Stop\n";
        }
    }

    ImGui::End();
    ImGui::PopStyleVar(3);
}

float ToolbarPanel::GetHeight()
{
    return m_Height;
}

void ToolbarPanel::SetHeight(float toolbarHeight)
{
    m_Height = toolbarHeight;
}

void ToolbarPanel::SetEditorApp(EditorApp* editorApp)
{
    m_editorApp = editorApp;
}
