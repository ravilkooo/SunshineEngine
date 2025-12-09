#pragma once

#include "Project.h"
#include <imgui.h>

#include "EASTL/shared_ptr.h"

namespace SE
{
    enum class SceneType
    {
        Custom = 0, 
        GAI = 1,
        Default = 2,   
        Parent = 3,      
        Lua = 4,         
        Resources = 5   
    };
    
    class ProjectSelector
    {
    public:
        ProjectSelector() {};
        ~ProjectSelector() = default;

        bool Show();
        void Close();

        SceneType GetSelectedSceneType() const { return m_selectedSceneType; }
        eastl::shared_ptr<SE::Project> GetSelectedProject() { return m_selectedProject; }
        void SetProjectList(SE::ProjectList* projects) { m_projects = projects; }

        bool IsVisible() const { return m_isVisible; }
        void SetVisible(bool isVisible) { m_isVisible = isVisible; }
        void ResetSelection()
        {
            m_selectedIndex = -1;
            m_selectedProject.reset();
            m_selectedSceneType = SceneType::Custom;
            m_lastError.clear();
        }
        void SetWindowSize(ImVec2 windowSize) { m_windowSize = windowSize; }

        // For testing
        static const char* SceneTypeToString(SceneType type);
        static const char* SceneTypeToDisplayName(SceneType type);
        static const wchar_t* SceneTypeToWString(SceneType type);

    private:
        void DrawWindow();
        void RefreshProjectList();
        bool CreateNewProject();
        void DrawEditPopup();
        void DrawDeletePopup();
        void DrawTestScenesPopup();
        bool RenameProject(int index, const eastl::string& newName);
        void DeleteProject(int index, bool deleteFilesFromDisk);

        ImVec2 m_windowSize;
        
        bool m_isVisible = true;
        eastl::shared_ptr<SE::Project> m_selectedProject;
        SE::ProjectList* m_projects = nullptr;
        int m_selectedIndex = -1;
        char m_newProjectName[256] = "";
        
        SceneType m_selectedSceneType = SceneType::Custom;

        int m_editingIndex = -1;
        char m_editBuffer[256] = "";
        
        int m_deletingIndex = -1;

        eastl::string m_lastError;
    };
}
