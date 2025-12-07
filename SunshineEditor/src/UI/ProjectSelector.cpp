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
        
        return !m_isVisible && m_selectedIndex >= 0;
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
        ImGui::SetNextWindowSize(m_windowSize);

        ImGui::Begin("Project Selector", NULL, 
                    ImGuiWindowFlags_NoCollapse | 
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove);

        ImGui::Text("Select Project");
        ImGui::Separator();

        if (ImGui::BeginChild("ProjectList", ImVec2(0, -120), true, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (!m_projects || m_projects->empty())
            {
                ImGui::Text("No projects found. Create a new one!");
            }
            else
            {
                for (int i = 0; i < static_cast<int>(m_projects->size()); ++i)
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

                        if (ImGui::Selectable(WStringToUtf8(displayName).c_str(), isSelected,
                            ImGuiSelectableFlags_AllowDoubleClick,
                            ImVec2(0, 20)))
                        {
                            m_selectedIndex = i;
                            m_selectedProject = eastl::make_shared<SE::Project>(project);
            
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
            m_selectedProject.reset();
        }

        ImGui::Separator();
        ImGui::Text("Create New Project");
        
        ImGui::InputText("Project Name", m_newProjectName, sizeof(m_newProjectName));
        static bool duplicate_error = false;
        if (ImGui::IsItemActive() || ImGui::IsItemEdited())
        {
            duplicate_error = false;
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Create") && strlen(m_newProjectName) > 0)
        {
            duplicate_error = !CreateNewProject();
            if (!duplicate_error)
            {
                m_newProjectName[0] = '\0';
            }
        }
        if (duplicate_error)
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Project already exists!");
        }
        ImGui::End();
    }

    void ProjectSelector::RefreshProjectList()
    {
        LoadProjects(*m_projects);
        m_selectedIndex = -1;
        m_selectedProject.reset();
    }

    bool ProjectSelector::CreateNewProject()
    {
        eastl::wstring projectName = Utf8ToWString(m_newProjectName);
        eastl::wstring projectPath = projectName + L"/";

        // Check duplicate
        for (const auto& p : *m_projects)
        {
            if (p.GetSubPath() == projectPath)
            {
                return false;
            }
        }

        eastl::wstring fullPath = JoinWchar_Wstring(PROJECTS_DIR, projectPath.c_str());
        if (std::filesystem::exists(fullPath.c_str()))
        {
            return false;
        }

        // Create directory
        try {
            std::filesystem::path dir(fullPath.c_str());
            std::filesystem::create_directories(dir);
        }
        catch (const std::exception& e) {
            return false;
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

        return true;
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
            }

            if (duplicate_error)
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
                    duplicate_error = !RenameProject(m_editingIndex, m_editBuffer);
                    if (!duplicate_error)
                    {
                        m_editingIndex = -1;
                        m_editBuffer[0] = '\0';
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
        
            ImGui::SameLine();
        
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                m_editingIndex = -1;
                m_editBuffer[0] = '\0';
                duplicate_error = false;
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
            if (m_deletingIndex >= 0 &&  static_cast<size_t>(m_deletingIndex) < m_projects->size())
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
        
                ImGui::Checkbox("Also delete project files from disk", &deleteFiles);
            }
        
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        
            if (ImGui::Button("Delete", ImVec2(120, 0)))
            {
                DeleteProject(m_deletingIndex, deleteFiles);
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

    bool ProjectSelector::RenameProject(int index, const eastl::string& newName)
    {
        if (index < 0 || static_cast<size_t>(index) >= m_projects->size())
            return false;
        
        eastl::wstring newPath = Utf8ToWString(newName) + L"/";
    
        for (size_t i = 0; i < m_projects->size(); ++i)
        {
            // if (i != static_cast<size_t>(index) && (*m_projects)[i].GetSubPath() == newPath)
            // {
            //     return false;
            // }
            if (static_cast<int>(i) == index) 
                continue;
            
            eastl::wstring existingPath = (*m_projects)[i].GetSubPath();
            
            eastl::wstring existingName = existingPath;
            if (!existingName.empty() && existingName.back() == L'/')
                existingName.pop_back();
            
            size_t slashPos = existingName.find_last_of(L'/');
            if (slashPos != eastl::wstring::npos)
                existingName = existingName.substr(slashPos + 1);
            
            if (existingName == Utf8ToWString(newName))
            {
                return false;   
            }
        }
        
        // (*m_projects)[index].SetSubPath(newPath);
        // SE::SaveProjects(*m_projects);
        //
        // if (index == m_selectedIndex && m_selectedProject)
        // {
        //     *m_selectedProject = (*m_projects)[index];
        // }
        
        eastl::wstring oldPath = (*m_projects)[index].GetSubPath();
        eastl::wstring oldFullPath = JoinWchar_Wstring(PROJECTS_DIR, oldPath.c_str());
        eastl::wstring newFullPath = JoinWchar_Wstring(PROJECTS_DIR, newPath.c_str());
        
        if (std::filesystem::exists(newFullPath.c_str()))
        {
            return false;
        }
    
        try {
            if (std::filesystem::exists(oldFullPath.c_str()))
            {
                std::filesystem::rename(oldFullPath.c_str(), newFullPath.c_str());
            }
        }
        catch (...) {
            return false;
        }
        
        (*m_projects)[index].SetSubPath(newPath);
        
        if (!SE::SaveProjects(*m_projects))
        {
            return false;
        }
        
        if (index == m_selectedIndex && m_selectedProject)
        {
            *m_selectedProject = (*m_projects)[index];
        }

        return true;
    }

    void ProjectSelector::DeleteProject(int index, bool deleteFilesFromDisk)
    {
        eastl::wstring fullPath = (*m_projects)[index].GetFullPath();
        
        if (index < 0 || static_cast<size_t>(index) >= m_projects->size())
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

        if (deleteFilesFromDisk)
        {
            try {
                std::filesystem::remove_all(fullPath.c_str());
            }
            catch (...) {}
        }
    }
}

