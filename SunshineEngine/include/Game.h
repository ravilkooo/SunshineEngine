#pragma once

// ThirdPartyLibs
#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

// SunshineLibs
#include "SunshineEngineAPI.h"
#include "GameTimer.h"

#include "Scene.h"
#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/DeferredRenderer.h>
#include <Windows/WindowsApp.h>


class SUNSHINE_ENGINE_API Game : public WindowsApp
{
public:
    Game();
    virtual ~Game();

    virtual void InitGame() {};
    virtual void Run();

    virtual void Update(float deltaTime) = 0;
    virtual void Render();

    GameTimer m_timer;
    Scene m_scene;
    eastl::unique_ptr<SE_G::DeferredRenderer> m_renderer;

    float m_deltaTime = 0.0f;
};

