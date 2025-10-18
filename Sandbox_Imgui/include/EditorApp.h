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
#include <Windows/DisplayWindow.h>
#include <GameTimer.h>


class EditorApp
{
    EditorApp();
    ~EditorApp();

    void Run();

    void Update(float deltaTime);
    void Render();

    eastl::unique_ptr<RenderingSystem> renderer;

    GameTimer timer;
    DisplayWindow displayWindow;

    HINSTANCE hInstance;
    LPCWSTR applicationName;

    int winWidth = 800;
    int winHeight = 800;

    float deltaTime = 0.0f;
};

