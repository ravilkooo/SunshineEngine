#pragma once

#include <d3d11.h>
// ThirdPartyLibs
#include <EASTL/string.h>
#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

#include <SunshineEngineAPI.h>

#include <Windows/InputDevice.h>

#include <GameTimer.h>

// #include <Utils/ILogManager.h>
// #include <GameObject/GameObjectFactory.h>
// #include <GameObject/GameObject.h>


// To-do: move lua manager from Editor to Engine
//#include <Scripting/LuaManager.h>

class PhysicsSystem;
class CharacterControllerSystem;
class AudioSystem;

namespace SE
{
    class ParticleSystem;
}

namespace SE_G
{
    struct DirectionalLightData;
    class RenderingSystem;
    class DeferredRenderer;
    class ShadowMapPass;
    class GPass;
    class LightPass;
    class TransparentPass;
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

    void InitializeAudio();

    void Run();

    void Start();
    void Stop();

    void ClearCachedAbsoluteTransforms();
    virtual void Update(float deltaTime);
    void ClearScene();

    bool LoadScene(const wchar_t* scenePath);
    bool LoadInputMapping(eastl::wstring inputMappingDir);

    void OnResize(UINT resizeWidth, UINT resizeHeight);

    // Input handling
    void HandleKeyDown(Keys key);
    void HandleKeyUp(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    UINT m_screenWidth = 800u;
    UINT m_screenHeight = 800u;

    GameTimer m_timer;
    //eastl::shared_ptr<Scene> m_scene;
    eastl::unique_ptr<SE_G::DeferredRenderer> m_renderer;
    eastl::unique_ptr<PhysicsSystem> m_physicsSystem;

    eastl::shared_ptr<SE_G::DirectionalLightData> lightData;
    SE_G::ShadowMapPass* m_shadowMapPass;
    SE_G::GPass* m_gPass;
    SE_G::LightPass* m_lightPass;
    SE_G::TransparentPass* m_transparentPass = nullptr;

    AudioSystem* m_audioSystem;

    float m_deltaTime = 0.0f;

    SE::ParticleSystem* m_particleSystem;

    eastl::unique_ptr<CharacterControllerSystem> m_characterControllerSystem;
};

