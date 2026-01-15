#pragma once

// ThirdPartyLibs
#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

#include <SunshineEngineAPI.h>

#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/DeferredRenderer.h>

#include <GameObject/GameObjectFactory.h>
#include <GameObject/GameObject.h>

#include <GameTimer.h>

#include <Scene.h>

#include <Physics/PhysicsSystem.h>

#include <Windows/InputDevice.h>

#include <Scripting/LuaManager.h>
#include <Utils/ILogManager.h>

// To-do: move lua manager from Editor to Engine
//#include <Scripting/LuaManager.h>

class PlayerObject;

namespace SE
{
    class ParticleSystem;
}

namespace SE_G
{
    class ShadowMapPass;
    class GPass;
    class LightPass;
}

class SUNSHINE_ENGINE_API Game
{
public:
    Game();
    virtual ~Game();
    
    void SetupRendering(
        eastl::shared_ptr<SE_G::RenderingSystem> renderSystem,
        UINT screenWidth = 800u,
        UINT screenHeight = 600u);
    void SetParticleSystem(eastl::shared_ptr<SE::ParticleSystem> ps);

    void SetupPhysics();

    void Run();

    void Start();
    void Stop();

    virtual void Update(float deltaTime);
    void ClearScene();

    bool LoadScene(const wchar_t* scenePath);
    
    bool LoadGAIScene();
    bool LoadDefaultScene();
    bool LoadParentScene();
    bool LoadLuaScene();
    bool LoadResourcesScene();

    void OnResize(UINT resizeWidth, UINT resizeHeight);

    // Input handling
    void HandleKeyDown(Keys key);
    void HandleKeyUp(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    UINT m_screenWidth = 800u;
    UINT m_screenHeight = 800u;

    GameTimer m_timer;
    eastl::shared_ptr<Scene> m_scene;
    eastl::unique_ptr<SE_G::DeferredRenderer> m_renderer;
    eastl::unique_ptr<PhysicsSystem> m_physicsSystem;

    LuaManager m_luaManager;

    eastl::shared_ptr<SE_G::DirectionalLightData> lightData;
    SE_G::ShadowMapPass* m_shadowMapPass;
    SE_G::GPass* m_gPass;
    SE_G::LightPass* m_lightPass;

    float m_deltaTime = 0.0f;

    // PlayerObject
    PlayerObject* m_playerObject;

    SE::ParticleSystem* m_particleSystem;

    // GAI
    void CreateGAIScene();
};

