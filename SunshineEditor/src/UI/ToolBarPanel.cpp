#include "UI/ToolBarPanel.h"
#include <iostream>
#include <ostream>
#include <Utils/StringUtils.h>
#include <EditorApp.h>
#include <Graphics/Renderer/DeferredRenderer.h>
#include <WorldEditor.h>

#include "Utils/FileDialogManager.h"

void ToolbarPanel::OnImGuiRender(float menuBarHeight)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, m_Height));
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 5));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 5));

    ImGui::Begin("Toolbar", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoBackground);

    float windowHeight = ImGui::GetWindowHeight();
    float buttonHeight = ImGui::GetFrameHeight(); 
    ImGui::SetCursorPosY((windowHeight - buttonHeight) / 2.0f);

    if (!isPlaying)
    {
        if (ImGui::Button("Add"))
        {
            ImGui::OpenPopup("AddMenuPopup");
        }
        
        if (ImGui::BeginPopup("AddMenuPopup"))
        {
            ShowAddMenu();
            ImGui::EndPopup();
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Play"))
        {
            isPlaying = true; 
            LOG_GAME_INFO("Game started");
            if (m_editorApp)
                m_editorApp->RunGame();
        }
        // ImGui::SameLine();
        // if (ImGui::Button("Save"))
        // {
        //     if (m_editorApp) {
        //         m_editorApp->SaveProject();
        //     }
        // }
    }
    else
    {
        if (!m_editorApp->m_gamePaused)
        {
            if (ImGui::Button("Pause"))
            {
                LOG_GAME_INFO("Game paused");
                m_editorApp->PauseGame();
            }
        }
        else
        {
            if (ImGui::Button("Continue"))
            {
                LOG_GAME_INFO("Game continued");
                m_editorApp->ContinueGame();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            isPlaying = false; 
            m_editorApp->StopGame();
            LOG_GAME_INFO("Game stopped");
        }
    }

    ImGui::End();
    ImGui::PopStyleVar(3);
}

float ToolbarPanel::GetHeight()
{
    return m_Height;
}

void ToolbarPanel::SetHeight(float toolbarHeight)
{
    m_Height = toolbarHeight;
}

void ToolbarPanel::SetScene(const eastl::shared_ptr<Scene_Info>& scene)
{
    m_scene = scene;
}

void ToolbarPanel::Init(EditorApp* editorApp)
{
    m_editorApp = editorApp;
    m_renderer = m_editorApp->m_worldEditor->m_renderer.get();
    m_camera = m_editorApp->m_worldEditor->m_renderer->m_mainCamera;
}

void ToolbarPanel::ShowAddMenu()
{
    if (ImGui::BeginMenu("Shape"))
    {
        if (ImGui::MenuItem("Plane")) 
        {
            m_editorApp->m_worldEditor->AddPlaneShape();
        }
        if (ImGui::MenuItem("Box")) 
        {
            m_editorApp->m_worldEditor->AddBoxShape();
        }
        if (ImGui::MenuItem("Sphere")) 
        {
            m_editorApp->m_worldEditor->AddSphereShape();
        }
        if (ImGui::MenuItem("Geosphere")) 
        {
            m_editorApp->m_worldEditor->AddGeosphereShape();
        }
        if (ImGui::MenuItem("Cylinder"))
        {
            m_editorApp->m_worldEditor->AddCylinderShape();
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Lightning"))
    {
        if (ImGui::MenuItem("SkyBox")) 
        {
            m_editorApp->m_worldEditor->AddSkyBox();
        }
        if (ImGui::MenuItem("Ambient")) 
        {
            m_editorApp->m_worldEditor->AddAmbientLight();
        }
        if (ImGui::MenuItem("Directional")) 
        {
            m_editorApp->m_worldEditor->AddDirectionalLight();
        }
        if (ImGui::MenuItem("Point Light")) 
        {
            m_editorApp->m_worldEditor->AddPointLight();
        }
        if (ImGui::MenuItem("Spot Light")) 
        {
            m_editorApp->m_worldEditor->AddSpotLight();
        }
        ImGui::EndMenu();
    }
    
    if (ImGui::MenuItem("Custom Mesh")) 
    {
        m_editorApp->m_worldEditor->AddCustomMesh();
    }
}
