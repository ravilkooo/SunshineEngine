#pragma once

#include "SunshineEngineAPI.h"
#include "GameTimer.h"
#include <EASTL/algorithm.h>

#include <GraphicsUtils/Scene.h>
#include <Physics/PhysicsEngine.h>
#include <RenderingSystem/RenderingSystem.h>
#include <Windows/DisplayWindow.h>


class SUNSHINE_ENGINE_API Game
{
public:
    Game();
    virtual ~Game();

    virtual void Run();

    virtual void Update(float deltaTime) = 0;
    virtual void Render();


    GameTimer timer;
    Scene scene;
    PhysicsEngine* physEngine;
    RenderingSystem* renderer;

    DisplayWindow displayWindow;

    HINSTANCE hInstance;
    LPCWSTR applicationName;

    int winWidth = 800;
    int winHeight = 800;

    float deltaTime = 0.0f;
};

