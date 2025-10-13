#pragma once

#include <Game.h>
#include <Graphics/DeferredRenderer.h>
#include <Graphics/GPass.h>
#include <Graphics/LightPass.h>
#include <Graphics/MainColorPass.h>

#include <Graphics/Lighting/LightCollection.h>
#include <Graphics/FullScreenQuad.h>

#include "TestCube.h"
#include <GameObject.h>
#include <Component/TransformComponent.h>
#include <Component/RenderComponent.h>

#include <windows.h>
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#define DEBUG_LIGHT_OBJECTS

class DeferredGame :
    public Game
{
public:
    DeferredGame();
    ~DeferredGame();

    void Update(float deltaTime) override;
    void Run() override;
    void Render() override;

    void HandleKeyDown(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);
    SpotLight* _sl_1;
    float currTime = 0.0f;
    DirectionalLight* _dl_1;

    // Particle test
    LightPass* gLightPass;
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Forward declare helper functions
LRESULT CALLBACK WndProcImGui(HWND, UINT, WPARAM, LPARAM);
