#pragma once

// ThirdPartyLibs
#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

// SunshineLibs
#include <Graphics/DeferredRenderer.h>

#include <GameObject.h>
#include <GameTimer.h>

#include <Graphics/GPass.h>
#include <Graphics/LightPass.h>

#include <GameObjectFactory.h>


class WorldEditor
{
public:
    WorldEditor();
    ~WorldEditor();

    void InitWorldEditor(
        eastl::shared_ptr<DeferredRenderer> renderer,
        UINT screenWidth = 800u,
        UINT screenHeight = 600u);
    void Run();

    void Update(float deltaTime);
    void Render();

    GameTimer m_timer;
    Scene m_scene;
    eastl::shared_ptr<DeferredRenderer> m_renderer;

    

    // Change to (Index + generation handle (robust for inserts/erases))
    // eastl::shared_ptr<GameObject> m_acticeGameObject;

    UINT m_screenWidth = 800;
    UINT m_screenHeight = 800;

    float m_deltaTime = 0.0f;
};

