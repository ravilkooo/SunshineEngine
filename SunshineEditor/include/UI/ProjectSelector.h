#pragma once

#include "Project.h"
#include <imgui.h>

#include "EASTL/shared_ptr.h"

namespace SE
{    
    class ProjectSelector
    {
    public:
        ProjectSelector() {
            if (!SE::LoadProjects(m_projectsList).empty())
            {
                SE::SaveProjects(m_projectsList);
            }
        };
        ~ProjectSelector() = default;

        bool Show();
        void Close();

        SE::Project* GetSelectedProject() { return m_selectedProject; }

        bool IsVisible() const { return m_isVisible; }
        void SetVisible(bool isVisible) { m_isVisible = isVisible; }
        void ResetSelection()
        {
            m_selectedIndex = -1;
            m_selectedProject = nullptr;
            m_lastError.clear();
        }
        void SetWindowSize(ImVec2 windowSize) { m_windowSize = windowSize; }

        SE::ProjectList m_projectsList = { SE::Project() };

        void RefreshProjectList();
    private:
        void DrawWindow();
        bool CreateNewProject();
        void DrawEditPopup();
        void DrawDeletePopup();
        bool RenameProject(int index, const eastl::string& newName);
        void DeleteProject(int index, bool deleteFilesFromDisk);

        ImVec2 m_windowSize;
        
        bool m_isVisible = true;
        SE::Project* m_selectedProject;
        //SE::ProjectList* m_projects = nullptr;
        int m_selectedIndex = -1;
        char m_newProjectName[256] = "";

        int m_editingIndex = -1;
        char m_editBuffer[256] = "";
        
        int m_deletingIndex = -1;

        eastl::string m_lastError;
    };
}
