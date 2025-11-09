#pragma once
#include <imgui.h>

class EditorApp;

class ToolbarPanel
{
public:
    void OnImGuiRender(float menuBarHeight);
    float getHeight();
    void setHeight(float toolbarHeight);

    bool isPlaying = false;
    
    void SetEditorApp(EditorApp* editorApp);
    
private:
    float m_ToolbarHeight = 25.0f;
    EditorApp* m_editorApp = nullptr;
};
