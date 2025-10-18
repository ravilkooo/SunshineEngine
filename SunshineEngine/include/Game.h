#pragma once

#include "SunshineEngineAPI.h"
#include "GameTimer.h"
#include <EASTL/algorithm.h>

#include "Scene.h"
#include <Graphics/RenderingSystem.h>
#include <Graphics/DeferredRenderer.h>
#include <Windows/DisplayWindow.h>


class SUNSHINE_ENGINE_API Game
{
public:
    Game();
    virtual ~Game();

    virtual void InitGame() {};
    virtual void Run();

    virtual void Update(float deltaTime) = 0;
    virtual void Render();


    GameTimer timer;
    Scene scene;
    DeferredRenderer* renderer;

    DisplayWindow displayWindow;

    HINSTANCE hInstance;
    LPCWSTR applicationName;

    int winWidth = 800;
    int winHeight = 800;

    float deltaTime = 0.0f;
};

