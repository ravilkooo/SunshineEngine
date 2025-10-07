#pragma once
#include <Game.h>
#include <RenderingSystem/DeferredRenderer.h>
#include <RenderingSystem/GBufferPass.h>
#include <RenderingSystem/LightPass.h>
#include <RenderingSystem/MainColorPass.h>

#include <LightObjects/LightCollection.h>
#include <GraphicsUtils/FullScreenQuad.h>

#include "TestCube.h"
#include <GameObject.h>
#include <Component/TransformComponent.h>

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
    void Render() override;

    void HandleKeyDown(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    ImGuiIO* io;
    SpotLight* _sl_1;
    float currTime = 0.0f;
    DirectionalLight* _dl_1;

    // Particle test
    LightPass* gLightPass;

    class MyGo : public GameObject {
    public:
        MyGo() {};

        void Tick(float deltaTime) override {
            return;
        }
    };

    // GameObject
    eastl::unique_ptr<GameObject> gobj;
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Forward declare helper functions
LRESULT CALLBACK WndProcImGui(HWND, UINT, WPARAM, LPARAM);
