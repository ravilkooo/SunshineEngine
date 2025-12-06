#pragma once

#include "Project.h"
#include <imgui.h>

#include "EASTL/shared_ptr.h"

namespace SE
{
    class ProjectSelector
    {
    public:
        ProjectSelector() {};
        ~ProjectSelector() = default;

        bool Show();
        void Close();
        
        eastl::shared_ptr<SE::Project> GetSelectedProject() { return m_selectedProject; }
        void SetProjectList(SE::ProjectList* projects) { m_projects = projects; }

        bool IsVisible() const { return m_isVisible; }

    private:
        void DrawWindow();
        void RefreshProjectList();
        void CreateNewProject();
        void DrawEditPopup();
        void DrawDeletePopup();
        void RenameProject(int index, const eastl::string& newName);
        void DeleteProject(int index);
        
        bool m_isVisible = true;
        eastl::shared_ptr<SE::Project> m_selectedProject;
        SE::ProjectList* m_projects = nullptr;
        int m_selectedIndex = -1;
        char m_newProjectName[256] = "";

        int m_editingIndex = -1;
        char m_editBuffer[256] = "";
        
        int m_deletingIndex = -1;
    };
}
