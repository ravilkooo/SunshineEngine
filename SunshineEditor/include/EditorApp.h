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
#include <Graphics/RenderingSystem.h>
#include <Windows/WindowsApp.h>
#include <GameTimer.h>
#include <Project.h>
#include <ResourceManager.h>
#include <WorldEditor.h>
#include <Game.h>
#include <Graphics/RenderPass.h>
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

class EditorApp : public WindowsApp
{
public:
    EditorApp();
    void InitEditorApp(UINT winWidth = 1600u, UINT winHeight = 800u);
    ~EditorApp();

    void Run();

    void Update(float deltaTime);
    void Render();
    void OnResize(UINT resizeWidth, UINT resizeHeight) override;
    void SetIcon(HWND hwnd) override;

    eastl::shared_ptr<DeferredRenderer> m_renderer;
    
    eastl::shared_ptr<WorldEditor> m_worldEditor;
    eastl::unique_ptr<Project> m_openedProject;
    eastl::unique_ptr<Game> m_currentGame;

    GameTimer m_timer;

    float m_deltaTime = 0.0f;

    sol::state m_lua;

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
    //

    bool is_layout_initialized = false;

    eastl::shared_ptr<ImguiEditorPass> imguiEditorPass;
    bool m_initialized = false;
};

