#pragma once

// ThirdPartyLibs
#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

#include <SunshineEngineAPI.h>

#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/Pass/GPass.h>
#include <Graphics/Renderer/Pass/LightPass.h>
#include <Graphics/Renderer/Pass/ShadowMapPass.h>

#include <GameObject/GameObjectFactory.h>
#include <GameObject/GameObject.h>

#include <GameTimer.h>

#include <Scene.h>

#include <Physics/PhysicsSystem.h>

#include <TracingSystem/TracingSystem.h>

#include <Utils/ILogManager.h>

// To-do: move lua manager from Editor to Engine
//#include <Scripting/LuaManager.h>


class SUNSHINE_ENGINE_API Game
{
public:
    Game();
    virtual ~Game();
    
    void SetupRendering(
        eastl::shared_ptr<SE_G::RenderingSystem> renderSystem,
        UINT screenWidth = 800u,
        UINT screenHeight = 600u);

    void SetupPhysics();

    void Run();

    void Start();
    void Stop();

    virtual void Update(float deltaTime);
    void Render();
    void ClearScene();

    bool LoadScene(const wchar_t* scenePath);

    bool LoadGAIScene();
    bool LoadDefaultScene();
    bool LoadParentScene();
    bool LoadLuaScene();
    bool LoadResourcesScene();

    void OnResize(UINT resizeWidth, UINT resizeHeight);

    UINT m_screenWidth = 800u;
    UINT m_screenHeight = 800u;

    GameTimer m_timer;
    eastl::shared_ptr<Scene> m_scene;
    eastl::unique_ptr<SE_G::DeferredRenderer> m_renderer;
    eastl::unique_ptr<PhysicsSystem> m_physicsSystem;
    
    // For Volodya
    //eastl::unique_ptr<TracingSystem> m_tracingSystem;

    // LuaManager m_luaManager;

    eastl::shared_ptr<SE_G::DirectionalLightData> lightData;
    SE_G::ShadowMapPass* m_shadowMapPass;
    SE_G::GPass* m_gPass;
    SE_G::LightPass* m_lightPass;

    float m_deltaTime = 0.0f;

    // GAI
    void CreateGAIScene();
};

