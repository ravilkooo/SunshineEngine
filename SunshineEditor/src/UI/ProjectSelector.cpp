#include "UI/ProjectSelector.h"

#include <filesystem>
#include <Utils/StringUtils.h>

#include "imgui_internal.h"

namespace SE
{
    bool ProjectSelector::Show()
    {
        if (!m_isVisible)
            return false;
        
        DrawWindow();
        
        return !m_isVisible;
    }

    void ProjectSelector::Close()
    {
        m_isVisible = false;
        m_lastError.clear();
    }

    void ProjectSelector::DrawWindow()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 center = viewport->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(m_windowSize);

        ImGui::Begin("Project Selector", NULL, 
                    ImGuiWindowFlags_NoCollapse | 
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove);

        ImGui::Text("Select Project");
        ImGui::Separator();

        if (!m_lastError.empty())
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: %s", m_lastError.c_str());
            ImGui::Separator();
        }

        if (ImGui::BeginChild("ProjectList", ImVec2(0, -120), true, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (m_projectsList.empty())
            {
                ImGui::Text("No projects found. Create a new one!");
            }
            else
            {
                for (int i = 0; i < static_cast<int>(m_projectsList.size()); ++i)
                {
                    auto& project = m_projectsList[i];
                    bool isSelected = (i == m_selectedIndex);
                    
                    eastl::wstring displayName = project.GetSubPath();
                    if (displayName.back() == L'/')
                        displayName.pop_back();

                    ImGui::PushID(i);

                    if (ImGui::BeginTable("ProjectRow", 2, 
                        ImGuiTableFlags_SizingStretchSame | 
                        ImGuiTableFlags_NoHostExtendX))
                    {
                        ImGui::TableSetupColumn("Project Name", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                        
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);

                        if (ImGui::Selectable(WStringToUtf8(displayName).c_str(), isSelected,
                            ImGuiSelectableFlags_AllowDoubleClick,
                            ImVec2(0, 20)))
                        {
                            m_selectedIndex = i;
                            m_selectedProject = &project;
            
                            if (ImGui::IsMouseDoubleClicked(0))
                            {
                                m_isVisible = false;
                            }
                        }

                        ImGui::TableSetColumnIndex(1);

                        ImGui::BeginGroup();
                        
                        float columnWidth = ImGui::GetColumnWidth();
                        float buttonsWidth = 60.0f + 70.0f + ImGui::GetStyle().ItemSpacing.x;
                        float buttonStartX = columnWidth - buttonsWidth;
        
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + buttonStartX);

                        
                        if (ImGui::Button("Edit", ImVec2(60, 20)))
                        {
                            m_editingIndex = i;
                            eastl::string displayNameUtf8 = WStringToUtf8(displayName);
                            strncpy_s(m_editBuffer, sizeof(m_editBuffer), 
                                      displayNameUtf8.c_str(), _TRUNCATE);
                            ImGui::OpenPopup("Edit Project");
                        }
        
                        DrawEditPopup();
        
                        ImGui::SameLine();
        
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.6f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 0.8f));
                        if (ImGui::Button("Delete", ImVec2(70, 20)))
                        {
                            m_deletingIndex = i;
                            ImGui::OpenPopup("Delete Project");
                        }
                        ImGui::PopStyleColor(2);
        
                        DrawDeletePopup();
                        
                        ImGui::EndGroup();
        
                        ImGui::EndTable();
                    }
                    
                    ImGui::PopID();
                    
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        
                        ImGui::Text("Created: %s", WStringToUtf8(eastl::wstring(FormatTime(project.GetCreationDate()).c_str())).c_str());
                        ImGui::Text("Last Saved: %s", WStringToUtf8(eastl::wstring(FormatTime(project.GetLastSavedTime()).c_str())).c_str());
                        
                        ImGui::EndTooltip();
                    }
                }
            }
        }
        ImGui::EndChild();

        ImGui::Spacing();
        
        if (ImGui::Button("Open Selected", ImVec2(120, 0)) && m_selectedIndex >= 0)
        {
            m_isVisible = false;
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Refresh", ImVec2(120, 0)))
        {
            RefreshProjectList();
        }

        ImGui::SameLine();
        
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            m_isVisible = false;
            m_selectedIndex = -1;
            m_selectedProject = nullptr;
        }

        ImGui::Separator();
        ImGui::Text("Create New Project");
        
        ImGui::InputText("Project Name", m_newProjectName, sizeof(m_newProjectName));
        ImGui::SameLine();
        
        if (ImGui::Button("Create") && strlen(m_newProjectName) > 0)
        {
            bool success = CreateNewProject();
            if (success)
            {
                m_newProjectName[0] = '\0';
                m_lastError.clear();
            }
        }

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 0.8f));
        if (ImGui::Button("[DEV] TESTING SCENES"))
        {
            ImGui::OpenPopup("Test Scenes");
        }
        ImGui::PopStyleColor(2);


        DrawTestScenesPopup();
        
        ImGui::End();
    }
    
    void ProjectSelector::DrawEditPopup()
    {
        if (ImGui::BeginPopupModal("Edit Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static bool duplicate_error = false;
            ImGui::Text("Rename project:");
            ImGui::InputText("##NewName", m_editBuffer, sizeof(m_editBuffer));

            if (ImGui::IsItemActive() || ImGui::IsItemEdited())
            {
                duplicate_error = false;
                m_lastError.clear();
            }

            if (!m_lastError.empty())
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", m_lastError.c_str());
            }
            else if (duplicate_error)
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Project name already exists!");
            }
        
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        
            if (ImGui::Button("Save", ImVec2(120, 0)))
            {
                if (strlen(m_editBuffer) > 0)
                {
                    bool success = RenameProject(m_editingIndex, m_editBuffer);
                    
                    if (success)
                    {
                        m_editingIndex = -1;
                        m_editBuffer[0] = '\0';
                        duplicate_error = false;
                        ImGui::CloseCurrentPopup();
                    }
                    else
                        duplicate_error = !success && m_lastError.empty();
                }
                else
                    m_lastError = "New project name cannot be empty";
            }
        
            ImGui::SameLine();
        
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                m_editingIndex = -1;
                m_editBuffer[0] = '\0';
                duplicate_error = false;
                m_lastError.clear();
                ImGui::CloseCurrentPopup();
            }
        
            ImGui::EndPopup();
        }
    }

    void ProjectSelector::DrawDeletePopup()
    {
        if (ImGui::BeginPopupModal("Delete Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static bool deleteFiles = false;
            if (m_deletingIndex >= 0 &&  static_cast<size_t>(m_deletingIndex) < m_projectsList.size())
            {
                eastl::wstring projectName = m_projectsList[m_deletingIndex].GetSubPath();
                if (projectName.back() == L'/')
                    projectName.pop_back();
                
                ImGui::Text("Delete project:");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "%s", 
                    WStringToUtf8(projectName).c_str());
                ImGui::Text("This will remove the project from the list.");
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), 
                    "Warning: Project files will NOT be deleted from disk!");
        
                ImGui::Checkbox("Also delete project files from disk", &deleteFiles);
            }

            if (!m_lastError.empty())
            {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: %s", m_lastError.c_str());
            }
        
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        
            if (ImGui::Button("Delete", ImVec2(120, 0)))
            {
                DeleteProject(m_deletingIndex, deleteFiles);
                if (m_lastError.empty())
                {
                    m_deletingIndex = -1;
                    ImGui::CloseCurrentPopup();
                }
            }
        
            ImGui::SameLine();
        
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                m_deletingIndex = -1;
                m_lastError.clear();
                ImGui::CloseCurrentPopup();
            }
        
            ImGui::EndPopup();
        }
    }

    void ProjectSelector::DrawTestScenesPopup()
    {
        if (ImGui::BeginPopupModal("Test Scenes", NULL, 
                                   ImGuiWindowFlags_AlwaysAutoResize | 
                                   ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::Text("Select Test Scene");
            ImGui::Separator();
            
            float windowWidth = ImGui::GetWindowSize().x;
            float buttonWidth = 150.0f;

            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
            if (ImGui::Button(SceneTypeToDisplayName(SceneType::Default), ImVec2(buttonWidth, 40)))
            {
                m_selectedSceneType = SceneType::Default;
                m_isVisible = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::Spacing();
            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
            if (ImGui::Button(SceneTypeToDisplayName(SceneType::GAI), ImVec2(buttonWidth, 40)))
            {
                m_selectedSceneType = SceneType::GAI;
                m_isVisible = false;
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::Spacing();
            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
            if (ImGui::Button(SceneTypeToDisplayName(SceneType::Parent), ImVec2(buttonWidth, 40)))
            {
                m_selectedSceneType = SceneType::Parent;
                m_isVisible = false;
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::Spacing();
            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
            if (ImGui::Button(SceneTypeToDisplayName(SceneType::Lua), ImVec2(buttonWidth, 40)))
            {
                m_selectedSceneType = SceneType::Lua;
                m_isVisible = false;
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::Spacing();
            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
            if (ImGui::Button(SceneTypeToDisplayName(SceneType::Resources), ImVec2(buttonWidth, 40)))
            {
                m_selectedSceneType = SceneType::Resources;
                m_isVisible = false;
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX((windowWidth - 100) * 0.5f);
            if (ImGui::Button("Cancel", ImVec2(100, 30)))
            {
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }
    }

    void ProjectSelector::RefreshProjectList()
    {
        eastl::string error = SE::LoadProjects(m_projectsList);
        if (!error.empty())
            m_lastError = "Failed to load project list: " + error;
        
        m_selectedIndex = -1;
        m_selectedProject = nullptr;
    }

    bool ProjectSelector::CreateNewProject()
    {
        if (strlen(m_newProjectName) == 0)
        {
            m_lastError = "Project name cannot be empty";
            return false;
        }

        Project newProject;
        
        eastl::string error = Project::CreateNew(m_newProjectName, newProject);
        
        if (!error.empty())
        {
            m_lastError = error; 
            return false;
        }
        
        RefreshProjectList();
        
        for (int i = 0; i < static_cast<int>(m_projectsList.size()); ++i)
        {
            if (m_projectsList[i].GetSubPath() == newProject.GetSubPath())
            {
                m_selectedIndex = i;
                m_selectedProject = &newProject;
                break;
            }
        }

        m_lastError.clear();
        return true;
    }
    
    bool ProjectSelector::RenameProject(int index, const eastl::string& newName)
    {
        if (index < 0 || static_cast<size_t>(index) >= m_projectsList.size())
        {
            m_lastError = "Invalid project index";
            return false;
        }

        Project* project = &m_projectsList[index];
        
        eastl::string error = project->Rename(newName);
        
        if (!error.empty())
        {
            m_lastError = error;
            return false;
        }

        RefreshProjectList();
        
        if (index == m_selectedIndex && m_selectedProject)
        {
            m_selectedProject = project;
        }

        m_lastError.clear();
        return true;
    }

    void ProjectSelector::DeleteProject(int index, bool deleteFilesFromDisk)
    {
        if (index < 0 || static_cast<size_t>(index) >= m_projectsList.size())
        {
            m_lastError = "Invalid project index";
            return;
        }
        
        Project projectToDelete = m_projectsList[index];
        
        eastl::string error = projectToDelete.Delete(deleteFilesFromDisk);
        
        if (!error.empty())
        {
            m_lastError = error;
            return;
        }
        
        RefreshProjectList();
        
        if (m_selectedIndex == index)
        {
            m_selectedIndex = -1;
            m_selectedProject = nullptr;
        }
        else if (m_selectedIndex > index)
        {
            m_selectedIndex--;
        }
    
        if (m_editingIndex == index)
            m_editingIndex = -1;
        else if (m_editingIndex > index)
            m_editingIndex--;
        
        if (m_deletingIndex == index)
            m_deletingIndex = -1;
        else if (m_deletingIndex > index)
            m_deletingIndex--;

        m_lastError.clear();
    }

    const char* ProjectSelector::SceneTypeToString(SceneType type)
    {
        switch (type)
        {
        case SceneType::Custom: return "Custom";
        case SceneType::GAI: return "GAI Testing";
        case SceneType::Default: return "Default Testing";
        case SceneType::Parent: return "Parent Testing";
        case SceneType::Lua: return "Lua Testing";
        case SceneType::Resources: return "Resources Testing";
        default: return "Default Testing";
        }
    }

    const char* ProjectSelector::SceneTypeToDisplayName(SceneType type)
    {
        switch (type)
        {
        case SceneType::GAI: return "GAI Testing Scene";
        case SceneType::Default: return "Default Testing Scene";
        case SceneType::Parent: return "Parent Testing Scene";
        case SceneType::Lua: return "Lua Scripting Testing Scene";
        case SceneType::Resources: return "Resources Testing Scene";
        default: return "Default Testing Scene";
        }
    }

    const wchar_t* ProjectSelector::SceneTypeToWString(SceneType type)
    {
        switch (type)
        {
        case SceneType::Custom: return L"Custom";
        case SceneType::GAI: return L"GAI Testing";
        case SceneType::Default: return L"Default Testing";
        case SceneType::Parent: return L"Parent Testing";
        case SceneType::Lua: return L"Lua Testing";
        case SceneType::Resources: return L"Resources Testing";
        default: return L"Default Testing";
        }
    }
}

