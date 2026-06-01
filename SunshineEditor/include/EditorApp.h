#pragma once

#include <SimpleMath.h>
namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

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
#include <Windows/WindowsApp.h>

#include <UI/ProjectSelector.h>
#include <Audio/AudioEditor.h>

class ImguiEditorPass;
class WorldEditor;
class Game;

namespace SE
{
    class Project;
}
namespace SE_G {
    class RenderingSystem;
    class RenderGroup;
}
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

    void InitResourceLoaders(ID3D11Device* device);

    void RunGame();
    void StopGame();

    void PauseGame();
    void ContinueGame();

    void UpdateEditor(float deltaTime);
    void UpdateGame(float deltaTime);

    void Render();
    void OnResize(UINT resizeWidth, UINT resizeHeight) override;
    void SetIcon(HWND hwnd) override;

    eastl::shared_ptr<SE_G::RenderingSystem> m_renderingSystem;
    eastl::unique_ptr<SE_G::RenderGroup> m_imguiRenderGroup;

    eastl::shared_ptr<WorldEditor> m_worldEditor;
    SE::Project* m_openedProject = nullptr;
    eastl::unique_ptr<Game> m_currentGame;

    float m_deltaTime = 0.0f;

	// Fixed timestep physics update
    float physicsUpdateFPS = 120.0f;
    float physicsUpdateMs = 1.0f / 120.0f;
    float accumulator = 0.0f;
    float accumulatorLimit = 4.0f / 120.0f;

    // FPS statitistic
    unsigned int frameCount = 0;
    float FPSstatisticTimer = 0;

    sol::state m_lua;

    enum class RuntimeMode {
        GAME_MODE, WORLD_EDITOR_MODE
    };
    RuntimeMode m_runtimeMode = RuntimeMode::WORLD_EDITOR_MODE;
    bool m_gamePaused = false;

private:
    void HandleKeyDown(Keys key);
    void HandleKeyUp(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    bool is_layout_initialized = false;

    ImguiEditorPass* imguiEditorPass = nullptr;
    bool m_initialized = false;

    bool m_projectSelected = false;
    eastl::unique_ptr<AudioEditor> m_audioEditor;
    AudioSystem* m_editorAudioSystem = nullptr;
private:
    // Only for testing
    // void ChooseProject();
    
    void SetupAssetsDirectory();
};
