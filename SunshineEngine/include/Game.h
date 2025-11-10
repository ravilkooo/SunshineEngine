#pragma once

// ThirdPartyLibs
#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

#include <SunshineEngineAPI.h>

#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/Pass/GPass.h>
#include <Graphics/Renderer/Pass/LightPass.h>

#include <GameObject.h>
#include <GameTimer.h>

#include <Scene.h>

#include <Windows/WindowsApp.h>


class SUNSHINE_ENGINE_API Game : public WindowsApp
{
public:
    Game();
    virtual ~Game();

    void InitGame(
        eastl::shared_ptr<SE_G::DeferredRenderer> renderer,
        UINT screenWidth = 800u,
        UINT screenHeight = 600u);

    virtual void Run();

    virtual void Update(float deltaTime);
    virtual void Render();

    void OnResize(UINT resizeWidth, UINT resizeHeight);

    UINT m_screenWidth = 800u;
    UINT m_screenHeight = 800u;

    GameTimer m_timer;
    Scene m_scene;
    eastl::shared_ptr<SE_G::DeferredRenderer> m_renderer;

    eastl::shared_ptr<SE_G::GPass> m_gPass;
    eastl::shared_ptr<SE_G::LightPass> m_lightPass;

    float m_deltaTime = 0.0f;
};

