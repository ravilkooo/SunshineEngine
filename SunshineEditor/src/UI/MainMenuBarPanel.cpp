#include "UI/MainMenuBarPanel.h"

void MainMenuBarPanel::OnImGuiRender()
{
    m_Height = 0.0f;

    if (ImGui::BeginMainMenuBar())
    {
        m_Height = ImGui::GetWindowSize().y;

        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("New", "Ctrl+N");
            ImGui::MenuItem("Open...", "Ctrl+O");
            ImGui::MenuItem("Save", "Ctrl+S");
            ImGui::Separator();
            ImGui::MenuItem("Exit");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::MenuItem("Undo", "Ctrl+Z");
            ImGui::MenuItem("Redo", "Ctrl+Y");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Build"))
        {
            ImGui::MenuItem("Build Solution");
            ImGui::MenuItem("Rebuild All");
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

