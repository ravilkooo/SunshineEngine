#pragma once
#include <Game.h>
#include <RenderingSystem/DeferredRenderer.h>
#include <RenderingSystem/GBufferPass.h>
#include <RenderingSystem/LightPass.h>
#include <RenderingSystem/MainColorPass.h>

#include <LightObjects/LightCollection.h>
#include <GraphicsUtils/FullScreenQuad.h>

#include "TestCube.h"

#include <windows.h>
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#define DEBUG_LIGHT_OBJECTS

class TestGameObjects :
    public Game
{
public:
    TestGameObjects();
    ~TestGameObjects();

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