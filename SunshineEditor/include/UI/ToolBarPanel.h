#pragma once
#include <imgui.h>

class ToolbarPanel
{
public:
    void OnImGuiRender(float menuBarHeight);
    float GetHeight();
    void SetHeight(float toolbarHeight);

    bool isPlaying = false;
    
    void SetEditorApp(EditorApp* editorApp);
    
private:
    float m_Height = 25.0f;
    EditorApp* m_editorApp = nullptr;
};
