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
        return !m_isVisible && m_selectedProject != nullptr;
    }

    void ProjectSelector::Close()
    {
        m_isVisible = false;
    }

    void ProjectSelector::DrawWindow()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 center = viewport->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Appearing);

        ImGui::Begin("Project Selector", &m_isVisible, 
                    ImGuiWindowFlags_NoCollapse | 
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove);

        ImGui::Text("Select Project");
        ImGui::Separator();

        if (ImGui::BeginChild("ProjectList", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (!m_projects || m_projects->empty())
            {
                ImGui::Text("No projects found. Create a new one!");
            }
            else
            {
                for (int i = 0; i < m_projects->size(); ++i)
                {
                    const auto& project = (*m_projects)[i];
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
                        ImGui::AlignTextToFramePadding();

                        if (ImGui::Selectable(WStringToUtf8(displayName).c_str(), isSelected,
                            ImGuiSelectableFlags_AllowDoubleClick,
                            ImVec2(0, 30)))
                        {
                            m_selectedIndex = i;
                            m_selectedProject = eastl::make_shared<SE::Project>(project);
            
                            if (ImGui::IsMouseDoubleClicked(0))
                            {
                                m_isVisible = false;
                            }
                        }

                        ImGui::TableSetColumnIndex(1);
                        ImGui::AlignTextToFramePadding();

                        ImGui::BeginGroup();
                        
                        float columnWidth = ImGui::GetColumnWidth();
                        float buttonsWidth = 60.0f + 70.0f + ImGui::GetStyle().ItemSpacing.x;
                        float buttonStartX = columnWidth - buttonsWidth;
        
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + buttonStartX);
                        
                        if (ImGui::Button("Edit", ImVec2(60, 0)))
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
                        if (ImGui::Button("Delete", ImVec2(70, 0)))
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
            m_selectedProject.reset();
        }

        ImGui::Separator();
        ImGui::Text("Create New Project");
        
        ImGui::InputText("Project Name", m_newProjectName, sizeof(m_newProjectName));
        ImGui::SameLine();
        
        if (ImGui::Button("Create") && strlen(m_newProjectName) > 0)
        {
            CreateNewProject();
            m_newProjectName[0] = '\0';
        }

        ImGui::End();
    }

    void ProjectSelector::RefreshProjectList()
    {
        LoadProjects(*m_projects);
        m_selectedIndex = -1;
        m_selectedProject.reset();
    }

    void ProjectSelector::CreateNewProject()
    {
        eastl::wstring projectName = Utf8ToWString(m_newProjectName);
        eastl::wstring projectPath = projectName + L"/";

        // Check duplicate
        for (const auto& p : *m_projects)
        {
            if (p.GetSubPath() == projectPath)
                return;
        }

        // Create directory
        eastl::wstring fullPath = JoinWchar_Wstring(PROJECTS_DIR, projectPath.c_str());
        try {
            std::filesystem::path dir(fullPath.c_str());
            std::filesystem::create_directories(dir);
        }
        catch (const std::exception& e) {
            return;
        }

        // Create default scene and save
        eastl::wstring sceneFile = JoinWchar_Wstring(fullPath.c_str(), L"scene.json");
        eastl::wstring templateFile = JoinWchar_Wstring(PROJECTS_DIR, L"Templates/DefaultScene.json");
        std::filesystem::copy(templateFile.c_str(), sceneFile.c_str());

        // Add to project list and save
        SE::Project newProject(projectPath);
        newProject.SetCreationDate(std::chrono::system_clock::now());
        newProject.SetLastSavedTime(std::chrono::system_clock::now());
        m_projects->push_back(newProject);
        SE::SaveProjects(*m_projects);
    
        m_selectedIndex = static_cast<int>(m_projects->size() - 1);
        m_selectedProject = eastl::make_shared<Project>(m_projects->back());
    }

    void ProjectSelector::DrawEditPopup()
    {
        if (ImGui::BeginPopupModal("Edit Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Rename project:");
            ImGui::InputText("##NewName", m_editBuffer, sizeof(m_editBuffer));
        
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        
            if (ImGui::Button("Save", ImVec2(120, 0)))
            {
                if (strlen(m_editBuffer) > 0)
                {
                    RenameProject(m_editingIndex, m_editBuffer);
                    m_editingIndex = -1;
                    m_editBuffer[0] = '\0';
                }
                ImGui::CloseCurrentPopup();
            }
        
            ImGui::SameLine();
        
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                m_editingIndex = -1;
                m_editBuffer[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
        
            ImGui::EndPopup();
        }
    }

    void ProjectSelector::DrawDeletePopup()
    {
        if (ImGui::BeginPopupModal("Delete Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (m_deletingIndex >= 0 && m_deletingIndex < m_projects->size())
            {
                eastl::wstring projectName = (*m_projects)[m_deletingIndex].GetSubPath();
                if (projectName.back() == L'/')
                    projectName.pop_back();
                
                ImGui::Text("Delete project:");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "%s", 
                    WStringToUtf8(projectName).c_str());
                ImGui::Text("This will remove the project from the list.");
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), 
                    "Warning: Project files will NOT be deleted from disk!");
            }
        
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        
            if (ImGui::Button("Delete", ImVec2(120, 0)))
            {
                DeleteProject(m_deletingIndex);
                m_deletingIndex = -1;
                ImGui::CloseCurrentPopup();
            }
        
            ImGui::SameLine();
        
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                m_deletingIndex = -1;
                ImGui::CloseCurrentPopup();
            }
        
            ImGui::EndPopup();
        }
    }

    void ProjectSelector::RenameProject(int index, const eastl::string& newName)
    {
        if (index < 0 || index >= m_projects->size())
            return;
        
        eastl::wstring newPath = Utf8ToWString(newName) + L"/";
    
        for (int i = 0; i < m_projects->size(); ++i)
        {
            if (i != index && (*m_projects)[i].GetSubPath() == newPath)
            {
                return;
            }
        }
        
        (*m_projects)[index].SetSubPath(newPath);
        SE::SaveProjects(*m_projects);
        
        if (index == m_selectedIndex && m_selectedProject)
        {
            *m_selectedProject = (*m_projects)[index];
        }
    }

    void ProjectSelector::DeleteProject(int index)
    {
        if (index < 0 || index >= m_projects->size())
            return;
        
        m_projects->erase(m_projects->begin() + index);
        SE::SaveProjects(*m_projects);
        
        if (m_selectedIndex == index)
        {
            m_selectedIndex = -1;
            m_selectedProject.reset();
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
    }
}

