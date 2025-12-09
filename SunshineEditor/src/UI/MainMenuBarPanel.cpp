#include "UI/MainMenuBarPanel.h"
#include "EditorApp.h"
#include "ImguiEditorPass.h"

void MainMenuBarPanel::OnImGuiRender()
{
    m_Height = 0.0f;

    if (ImGui::BeginMainMenuBar())
    {
        m_Height = ImGui::GetWindowSize().y;

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
                m_showNewProjectPopup = true;
            }
            
            if (ImGui::MenuItem("Open...", "Ctrl+O"))
            {
                m_showOpenProjectPopup = true;
            }
            
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                if (m_editorApp)
                {
                    m_editorApp->SaveProject();
                }
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Exit"))
            {
                m_showExitPopup = true;
            }
            
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
    
    DrawNewProjectPopup();
    DrawOpenProjectPopup();
    DrawExitPopup();
}

void MainMenuBarPanel::DrawNewProjectPopup()
{
    if (m_showNewProjectPopup)
    {
        ImGui::OpenPopup("New Project");
        m_showNewProjectPopup = false;
    }
    
    if (ImGui::BeginPopupModal("New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Do you want to create a new project?");
        
        if (m_editorApp && m_editorApp->m_openedProject)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), 
                "Current project will be closed!");
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        float buttonWidth = 120.0f;
        float windowWidth = ImGui::GetWindowSize().x;
        float spacing = (windowWidth - buttonWidth * 3 - ImGui::GetStyle().ItemSpacing.x * 2) / 2;
        
        ImGui::SetCursorPosX(spacing);
        
        if (ImGui::Button("Save and Create", ImVec2(buttonWidth, 0)))
        {
            SaveCurrentProject();
            CloseCurrentProject();
            
            if (m_editorApp && m_imguiEditorPass)
            {
                m_imguiEditorPass->SetProjectSelectorVisible();
            }
            
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.7f, 0.8f));
        if (ImGui::Button("Create Without Save", ImVec2(buttonWidth, 0)))
        {
            CloseCurrentProject();
            
            if (m_editorApp && m_imguiEditorPass)
            {
                m_imguiEditorPass->SetProjectSelectorVisible();
            }
            
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(2);
        
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void MainMenuBarPanel::DrawOpenProjectPopup()
{
    if (m_showOpenProjectPopup)
    {
        ImGui::OpenPopup("Open Project");
        m_showOpenProjectPopup = false;
    }
    
    if (ImGui::BeginPopupModal("Open Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Do you want to open another project?");
        
        if (m_editorApp && m_editorApp->m_openedProject)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), 
                "Current project will be closed!");
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        float buttonWidth = 120.0f;
        float windowWidth = ImGui::GetWindowSize().x;
        float spacing = (windowWidth - buttonWidth * 3 - ImGui::GetStyle().ItemSpacing.x * 2) / 2;
        
        ImGui::SetCursorPosX(spacing);
        
        if (ImGui::Button("Save and Open", ImVec2(buttonWidth, 0)))
        {
            SaveCurrentProject();
            CloseCurrentProject();
            
            if (m_editorApp && m_imguiEditorPass)
            {
                m_imguiEditorPass->SetProjectSelectorVisible();
            }
            
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.7f, 0.8f));
        if (ImGui::Button("Open Without Save", ImVec2(buttonWidth, 0)))
        {
            CloseCurrentProject();
            
            if (m_editorApp && m_imguiEditorPass)
            {
                m_imguiEditorPass->SetProjectSelectorVisible();
            }
            
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(2);
        
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void MainMenuBarPanel::DrawExitPopup()
{
    if (m_showExitPopup)
    {
        ImGui::OpenPopup("Exit Editor");
        m_showExitPopup = false;
    }
    
    if (ImGui::BeginPopupModal("Exit Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to exit?");
        
        if (m_editorApp && m_editorApp->m_openedProject)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), 
                "Current project has unsaved changes!");
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        float buttonWidth = 120.0f;
        float windowWidth = ImGui::GetWindowSize().x;
        float spacing = (windowWidth - buttonWidth * 3 - ImGui::GetStyle().ItemSpacing.x * 2) / 2;
        
        ImGui::SetCursorPosX(spacing);
        
        if (ImGui::Button("Save and Exit", ImVec2(buttonWidth, 0)))
        {
            SaveCurrentProject();
            
            if (m_editorApp)
            {
                PostQuitMessage(0);
            }
            
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 0.8f));
        if (ImGui::Button("Exit Without Save", ImVec2(buttonWidth, 0)))
        {
            if (m_editorApp)
            {
                PostQuitMessage(0);
            }
            
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(2);
        
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void MainMenuBarPanel::CloseCurrentProject()
{
    if (m_editorApp)
    {
        if (m_editorApp->m_runtimeMode == EditorApp::RuntimeMode::GAME_MODE)
        {
            m_editorApp->StopGame();
        }
        
        if (m_editorApp->m_worldEditor)
        {
            m_editorApp->m_worldEditor->ClearScene();
        }
        
        m_editorApp->m_openedProject.reset();
        
        
        if (m_imguiEditorPass)
        {
            m_imguiEditorPass->m_ProjectSelected = false;
        }
        m_editorApp->CloseProject();
    }
}

void MainMenuBarPanel::SaveCurrentProject()
{
    if (m_editorApp && m_editorApp->m_openedProject)
    {
        m_editorApp->SaveProject();
    }
}

