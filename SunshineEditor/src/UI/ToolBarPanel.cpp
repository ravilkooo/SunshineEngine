#include "UI/ToolBarPanel.h"
#include <iostream>
#include <ostream>
#include <Utils/StringUtils.h>
#include <EditorApp.h>
#include <GameObject/EditorObjectFactory.h>
#include <ParticleSystem/ParticleEmitter.h>
#include <SceneHierarchy.h>

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
        if (ImGui::MenuItem("Box")) 
        {
            AddBoxShape();
        }
        if (ImGui::MenuItem("Sphere")) 
        {
            AddSphereShape();
        }
        if (ImGui::MenuItem("Geosphere")) 
        {
            AddGeosphereShape();
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Lightning"))
    {
        if (ImGui::MenuItem("SkyBox")) 
        {
            AddSkyBox();
        }
        if (ImGui::MenuItem("Ambient")) 
        {
            AddAmbientLight();
        }
        if (ImGui::MenuItem("Directional")) 
        {
            AddDirectionalLight();
        }
        if (ImGui::MenuItem("Point Light")) 
        {
            AddPointLight();
        }
        if (ImGui::MenuItem("Spot Light")) 
        {
            AddSpotLight();
        }
        ImGui::EndMenu();
    }
    
    if (ImGui::MenuItem("Custom Mesh")) 
    {
        AddCustomMesh();
    }

    if (ImGui::MenuItem("Particle Emitter"))
    {
        AddParticleEmitter();
    }
}

void ToolbarPanel::AddBoxShape()
{
    if (m_editorApp && m_renderer && m_scene)
    {
        auto boxObject = EditorObjectFactory::CreateBoxObject(m_renderer, 1.0f, 1.0f, 1.0f);
        
        if (boxObject)
        {
            auto uuid = m_scene->AddGameObject(std::move(boxObject));
			m_scene->m_sceneGraph->Add(uuid);
        }
    }
}

void ToolbarPanel::AddSphereShape()
{
    if (m_editorApp && m_renderer && m_scene)
    {
        auto sphereObject = EditorObjectFactory::CreateSphereObject(m_renderer, 1.0f);
        
        if (sphereObject)
        {
            auto uuid = m_scene->AddGameObject(std::move(sphereObject));
            m_scene->m_sceneGraph->Add(uuid);
        }
    }
}

void ToolbarPanel::AddGeosphereShape()
{
    if (m_editorApp && m_renderer && m_scene)
    {
        auto geosphereObject = EditorObjectFactory::CreateGeosphereObject(m_renderer, 1.0f);
        
        if (geosphereObject)
        {
            auto uuid = m_scene->AddGameObject(std::move(geosphereObject));
            m_scene->m_sceneGraph->Add(uuid);
        }
    }
}

void ToolbarPanel::AddSkyBox()
{
    if (m_editorApp && m_renderer && m_scene)
    {
        if (m_camera)
        {
            auto skyboxObject = EditorObjectFactory::CreateSkyBox(m_renderer, m_camera);
            
            if (skyboxObject)
            {
                auto uuid = m_scene->AddGameObject(std::move(skyboxObject));
                m_scene->m_sceneGraph->Add(uuid);
            }
        }
    }
}

void ToolbarPanel::AddAmbientLight()
{
    if (m_editorApp && m_renderer && m_scene)
    {
        if (m_camera)
        {
            auto ambientLightObject = EditorObjectFactory::CreateAmbientLightObject(m_renderer, m_camera);
            
            if (ambientLightObject)
            {
                auto uuid = m_scene->AddGameObject(std::move(ambientLightObject));
                m_scene->m_sceneGraph->Add(uuid);
            }
        }
    }
}

void ToolbarPanel::AddDirectionalLight()
{
    if (m_editorApp && m_renderer && m_scene)
    {
        if (m_camera)
        {
            auto directionalLightObject = EditorObjectFactory::CreateDirectionalLightObject(m_renderer, m_camera,
                {
                    DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
                    DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
                    DXSM::Vector3::Zero, 0,
                    DXSM::Vector2(0, -DX::XM_PIDIV4), 0, 0
                });
            
            if (directionalLightObject)
            {
                auto uuid = m_scene->AddGameObject(std::move(directionalLightObject));
                m_scene->m_sceneGraph->Add(uuid);
            }
        }
    }
}

void ToolbarPanel::AddPointLight()
{
    if (m_editorApp && m_renderer && m_scene)
    {
        if (m_camera)
        {
            auto pointLightObject = EditorObjectFactory::CreatePointLightObject(m_renderer, m_camera,
                {
                    DXSM::Vector3::One, 1.0f,
                    DXSM::Vector3::One, 1.0f,
                    DXSM::Vector3::Zero, 20,
                    DXSM::Vector3::One, 0
                });
            
            if (pointLightObject)
            {
                auto uuid = m_scene->AddGameObject(std::move(pointLightObject));
                m_scene->m_sceneGraph->Add(uuid);
            }
        }
    }
}

void ToolbarPanel::AddSpotLight()
{
    if (m_editorApp && m_renderer && m_scene)
    {
        if (m_camera)
        {
            auto spotLightObject = EditorObjectFactory::CreateSpotLightObject(m_renderer, m_camera,
                {
                    DXSM::Vector3::One, 1.0f,
                    DXSM::Vector3::One, 1.0f,
                    DXSM::Vector3::Zero, 20,
                    DXSM::Vector2(0, -DX::XM_PIDIV4), 10, 0,
                    DXSM::Vector3::One, 0
                });
            
            if (spotLightObject)
            {
                auto uuid = m_scene->AddGameObject(std::move(spotLightObject));
                m_scene->m_sceneGraph->Add(uuid);
            }
        }
    }
}

void ToolbarPanel::AddCustomMesh()
{
    if (m_renderer && m_scene)
    {
        // auto filters = FileDialogManager::Get3DModelFilters();
        // auto modelPath = FileDialogManager::Get().OpenFile(
        //     L"Select 3D Model",
        //     filters,
        //     ContentBrowserPanel::s_AssetsDirectory
        // );
        //
        // if (!modelPath.empty())
        // {
        //     eastl::string filePath = FileDialogManager::ToUTF8(modelPath.wstring()).c_str();
        //     
        //     auto customMeshObject = EditorObjectFactory::CreateCustomMesh(m_renderer, filePath);
        //     
        //     if (customMeshObject)
        //     {
        //         m_scene->AddGameObject(std::move(customMeshObject));
        //     }
        // }

        auto customMeshObject = EditorObjectFactory::CreateCustomMesh(
            m_renderer,
            AssetPath(L"Box"));
            
        if (customMeshObject)
        {
            auto uuid = m_scene->AddGameObject(std::move(customMeshObject));
            m_scene->m_sceneGraph->Add(uuid);
        }
    }
    else
    {
        LOG_EDITOR_ERROR("Cannot add Custom Mesh: Renderer or Scene not initialized");
    }
}


void ToolbarPanel::AddParticleEmitter()
{
    if (m_renderer && m_scene)
    {
        auto particleEmitter = EditorObjectFactory::CreateParticleEmitter(
            m_renderer->m_particleSystem.get());

        if (particleEmitter)
        {
            auto uuid = m_scene->AddGameObject(std::move(particleEmitter));
            m_scene->m_sceneGraph->Add(uuid);
        }
    }
    else
    {
        LOG_EDITOR_ERROR("Cannot add Custom Mesh: Renderer or Scene not initialized");
    }
}