#pragma once
#include <imgui.h>

class EditorApp;
class Project;
class ImguiEditorPass;

class MainMenuBarPanel
{
public:
    void OnImGuiRender(); 
    float GetHeight() const { return m_Height; }

    void SetEditorApp(EditorApp* editorApp) { m_editorApp = editorApp; }
    void SetImguiEditorPass(ImguiEditorPass* imguiEditorPass) { m_imguiEditorPass = imguiEditorPass; }


private:
    float m_Height = 0.0f;
    EditorApp* m_editorApp = nullptr;
    ImguiEditorPass* m_imguiEditorPass = nullptr;

    bool m_showNewProjectPopup = false;
    bool m_showOpenProjectPopup = false;
    bool m_showExitPopup = false;

    void DrawNewProjectPopup();
    void DrawOpenProjectPopup();
    void DrawExitPopup();
    
    void CloseCurrentProject();
    void SaveCurrentProject();
};
