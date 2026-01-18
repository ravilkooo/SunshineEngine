#pragma once
#include <imgui.h>

#include "Scene.h"
#include "Graphics/Renderer/RenderingSystem.h"

class EditorApp;

class ToolbarPanel
{
public:
    void OnImGuiRender(float menuBarHeight);
    float GetHeight();
    void SetHeight(float toolbarHeight);

    void SetScene(const eastl::shared_ptr<Scene_Info>& scene);

    bool isPlaying = false;
    
    void Init(EditorApp* editorApp);
    
private:
    float m_Height = 25.0f;
    EditorApp* m_editorApp = nullptr;
    eastl::shared_ptr<Scene_Info> m_scene = nullptr;
    SE_G::DeferredRenderer* m_renderer = nullptr;
    eastl::shared_ptr<SE_G::Camera> m_camera = nullptr;

    void ShowAddMenu();
    
    void AddBoxShape();
    void AddSphereShape();
    void AddGeosphereShape();
    void AddSkyBox();
    void AddAmbientLight();
    void AddDirectionalLight();
    void AddPointLight();
    void AddSpotLight();
    void AddCustomMesh();
    void AddParticleEmitter();
};
