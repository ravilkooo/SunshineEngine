#pragma once

// ThirdPartyLibs
#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

#include <sol/sol.hpp>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>

#include <sol_ImGui.h>

// SunshineLibs
#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/RenderGroup.h>
#include <Graphics/Renderer/Pass/RenderPass.h>

#include <Windows/WindowsApp.h>

//#include <ResourceManager.h>
#include <GameTimer.h>
#include <Game.h>

#include <Project.h>
#include <WorldEditor.h>
#include <ImguiEditorPass.h>

enum class MoveKey
{
    W,
    S, 
    D, 
    A,
    Shift, 
    Ctrl 
};

struct Ray
{
    DX::XMVECTOR Origin;
    DX::XMVECTOR Direction;
};

class EditorApp : public WindowsApp
{
public:

    EditorApp();
    void InitEditorApp(UINT winWidth = 1600u, UINT winHeight = 800u);
    ~EditorApp();

    // Open project from projectlist
    bool OpenProject();
    void CloseProject();

    // Save openedProject
    void SaveProject();

    // Create new project (create folder, scene.json, and add it to projectlist)
    // void CreateProject();

    // Add existing project to projectlist (console-driven)
    // void AddProject();

    // Remove project from projectlist (console-driven)
    // void RemoveProject();

    void RunApp();

    void RunGame();
    void StopGame();

    void PauseGame();
    void ContinueGame();

    void UpdateEditor(float deltaTime);
    void UpdateGame(float deltaTime);

    void Render();
    void OnResize(UINT resizeWidth, UINT resizeHeight) override;
    void SetIcon(HWND hwnd) override;

    SE::SceneType m_loadedSceneType = SE::SceneType::Custom;

    SE::ProjectList m_projectsList = { SE::Project() };

    eastl::shared_ptr<SE_G::RenderingSystem> m_renderingSystem;
    
    eastl::shared_ptr<WorldEditor> m_worldEditor;
    eastl::shared_ptr<SE::Project> m_openedProject;
    eastl::unique_ptr<Game> m_currentGame;

    GameTimer m_timer;

    float m_deltaTime = 0.0f;

    sol::state m_lua;

    enum class RuntimeMode {
        GAME_MODE, WORLD_EDITOR_MODE
    };
    RuntimeMode m_runtimeMode = RuntimeMode::WORLD_EDITOR_MODE;
    bool m_gamePaused = false;

private:
    // Camera movings
    void HandleKeyDown(Keys key);
    void HandleKeyUp(Keys key);

    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    bool MovingPressed[6] = { false };

    float CameraSpeed = 20.0f;
    float const MaxCameraSpeed = 100.0f;
    float const MinCameraSpeed = 10.0f;
    float const CameraSpeedStep = 10.0f;

    bool IsRightMousePressed = false;

    float const CameraRotateSpeed = 0.5f;

    bool is_layout_initialized = false;

    eastl::unique_ptr<SE_G::RenderGroup> m_imguiRenderGroup;
    ImguiEditorPass* imguiEditorPass;
    bool m_initialized = false;

    bool m_projectSelected = false;

private:
    // Only for testing
    // void ChooseProject();
    
    bool LoadTestScene(SE::SceneType sceneType);
    void SetupAssetsDirectory();
};
