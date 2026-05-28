#pragma once

#include <d3d11.h>
#include <wrl.h>

// ThirdPartyLibs
#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

#include <Scene.h>

#include <Utils/UUID.h>

#include <GameTimer.h>

#include <Scripting/LuaManager.h>

#include <LogManager.h>

#include <Windows/InputManager.h>
#include <Windows/InputDevice.h>

#include <SimpleMath.h>
namespace DXSM = DirectX::SimpleMath;

namespace SE
{
    class ParticleSystem;
}

namespace SE_G
{
    class RenderingSystem;
    class DeferredRenderer;

    class GPass;
    class GBuffer;
    class LightPass;
    class SelectionPass;
    class IconPass;
    class ColliderPass;
    class TriggerPass;
    class EmitterDebugPass;
    class PerceptionDebugPass;
};

struct Selection {
    eastl::unordered_set<SE::UUID> picked;
    SE::UUID last_clicked = SE::UUID(0u);

    bool Contains(const SE::UUID n) const { return picked.find(n) != picked.end(); }
    void SetSingle(const SE::UUID n) { picked.clear(); picked.insert(n); last_clicked = n; }
    void Toggle(const SE::UUID n) { if (!picked.erase(n)) picked.insert(n); last_clicked = n; }
};

class WorldEditor
{
public:

    struct PixelInfo
    {
        DXSM::Vector3 worldPos;
        uint32_t hi;
        DXSM::Vector3 worldNormal;
        uint32_t lo;
    };

    class PixelInfoHandler {
    public:

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_clickMouseBuffer;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_UUIDOnMouseClickShader;

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_outputUUIDBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_outputUUIDBufferStaged;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_outputUUIDUAV;

        PixelInfoHandler();

        ~PixelInfoHandler();

        void Init(ID3D11Device* device);

        PixelInfo GetPixelInfo(ID3D11DeviceContext* context,
            eastl::shared_ptr<SE_G::GBuffer> gbuffer,
            UINT mouseClickX, UINT mouseClickY);

    };

    PixelInfoHandler* m_pixelUUIDHandler;

    WorldEditor();
    ~WorldEditor();

    void SetupRendering(
        eastl::shared_ptr<SE_G::RenderingSystem> renderSystem,
        UINT screenWidth = 800u,
        UINT screenHeight = 600u);
    void SetUpPlayerObject();

    void InitResourceLoaders(ID3D11Device* device);

    // Start/Pause worldEditor and it's rendering
    void Start();
    void Pause();

    void Update(float deltaTime);
    //void SyncronizeTransforms();
    void CloseProject();
    void ClearScene();

    void SaveScene(const wchar_t* scenePath);
    bool LoadScene(const wchar_t* scenePath);

    /*
    void SavePlayer(const wchar_t* playerPath);
    bool LoadPlayer(const wchar_t* playerPath);
    */

    void OnResize(UINT resizeWidth, UINT resizeHeight);

    // void DeprojectScreenToWorld(DXSM::Vector2 mouseScreenCoords, DXSM::Vector2 lastGameViewportSize);

    PixelInfo GetPixelInfo(UINT x, UINT y);
    SE::UUID ChooseObjectByClick(UINT x, UINT y);

    SE_G::RenderingSystem* m_renderingSystem;

    GameTimer m_timer;
    eastl::shared_ptr<Scene_Info> m_scene;
    eastl::unique_ptr<SE_G::DeferredRenderer> m_renderer;
    LuaManager m_luaManager;

    UINT m_screenWidth = 800u;
    UINT m_screenHeight = 800u;

    SE_G::GPass* m_gPass = nullptr;
    SE_G::LightPass* m_lightPass = nullptr;
    SE_G::SelectionPass* m_selectionPass = nullptr;
    SE_G::IconPass* m_iconPass = nullptr;
    SE_G::ColliderPass* m_colliderPass = nullptr;
    SE_G::TriggerPass* m_triggerPass = nullptr;
    SE_G::EmitterDebugPass* m_emitterPass = nullptr;
    SE_G::PerceptionDebugPass* m_perceptionPass = nullptr;

    float m_deltaTime = 0.0f;

    // Hierarchy
    Selection m_hierarchySelection;

    // PlayerObject
    SE::UUID m_playerObject = SE::UUID(0u);

    SE::ParticleSystem* m_particleSystem;

    // Robust input system for editor camera
    InputManager m_editorInputManager;
    // Input handling
    void HandleKeyDown(Keys key);
    void HandleKeyUp(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    bool IsRightMousePressed = false;
    float CameraSpeed = 20.0f;
    float const MaxCameraSpeed = 100.0f;
    float const MinCameraSpeed = 10.0f;
    float const CameraSpeedStep = 10.0f;
    float const CameraRotateSpeed = 0.5f;

    // Context menu and tool bar panel
    void AddBoxShape(DXSM::Vector3 initPos = DXSM::Vector3::Zero);
    void AddPlaneShape(DXSM::Vector3 initPos = DXSM::Vector3::Zero);
    void AddSphereShape(DXSM::Vector3 initPos = DXSM::Vector3::Zero);
    void AddGeosphereShape(DXSM::Vector3 initPos = DXSM::Vector3::Zero);
    void AddCylinderShape(DXSM::Vector3 initPos = DXSM::Vector3::Zero);
    void AddSkyBox(DXSM::Vector3 initPos = DXSM::Vector3::Zero);
    void AddAmbientLight(DXSM::Vector3 initPos = DXSM::Vector3::Zero);
    void AddDirectionalLight(DXSM::Vector3 initPos = DXSM::Vector3::Zero);
    void AddPointLight(DXSM::Vector3 initPos = DXSM::Vector3::Zero);
    void AddSpotLight(DXSM::Vector3 initPos = DXSM::Vector3::Zero);
    void AddCustomMesh(DXSM::Vector3 initPos = DXSM::Vector3::Zero);
    void AddParticleEmitter(DXSM::Vector3 initPos = DXSM::Vector3::Zero);

    // Copy/Paste objects
    SE::UUID m_copiedObjUUID = SE::UUID(0u);
    json m_copiedObjSerialized = {};
private:
    //eastl::shared_ptr<PhysicsSystem> m_physicsSystem;
    // testing
    // SE::UUID floorId;
    // SE::UUID ballId;
};
