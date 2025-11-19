#pragma once

// ThirdPartyLibs
#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/DeferredRenderer.h>

#include <GameObject/GameObject.h>
#include <GameTimer.h>

#include <Graphics/Renderer/Pass/GPass.h>
#include <Graphics/Renderer/Pass/LightPass.h>
#include <Graphics/Renderer/Pass/SelectionPass.h>
#include <Graphics/Renderer/Pass/IconPass.h>

#include <GameObject/EditorObjectFactory.h>
#include <Scripting/LuaManager.h>

#include <Physics/PhysicsSystem.h>
#include <LogManager.h>


class WorldEditor
{
public:

    class PixelUUIDHandler {
    public:

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_clickMouseBuffer;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_UUIDOnMouseClickShader;

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_outputUUIDBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_outputUUIDBufferStaged;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_outputUUIDUAV;

        PixelUUIDHandler();

        ~PixelUUIDHandler();

        void Init(ID3D11Device* device);

        SE::UUID GetUUID(ID3D11DeviceContext* context,
            ID3D11ShaderResourceView* UUIDTextureView,
            UINT mouseClickX, UINT mouseClickY);

    };

    PixelUUIDHandler* m_pixelUUIDHandler;

    WorldEditor();
    ~WorldEditor();

    void SetupRendering(
        eastl::shared_ptr<SE_G::RenderingSystem> renderSystem,
        UINT screenWidth = 800u,
        UINT screenHeight = 600u);

    // Start/Pause worldEditor and it's rendering
    void Start();
    void Pause();

    void Update(float deltaTime);
    //void SyncronizeTransforms();
    void Render();
    void ClearScene();
    
    void InitScene();
    void SaveScene(const wchar_t* scenePath);
    bool LoadScene(const wchar_t* scenePath);

    void OnResize(UINT resizeWidth, UINT resizeHeight);

    // void DeprojectScreenToWorld(DXSM::Vector2 mouseScreenCoords, DXSM::Vector2 lastGameViewportSize);

    SE::UUID ChooseObjectByClick(UINT x, UINT y);

    GameTimer m_timer;
    eastl::shared_ptr<Scene_Info> m_scene;
    eastl::unique_ptr<SE_G::DeferredRenderer> m_renderer;
    LuaManager m_luaManager;

    // Change to (Index + generation handle (robust for inserts/erases))
    // eastl::shared_ptr<GameObject> m_acticeGameObject;

    UINT m_screenWidth = 800u;
    UINT m_screenHeight = 800u;

    SE_G::GPass* m_gPass;
    SE_G::LightPass* m_lightPass;
    SE_G::SelectionPass* m_selectionPass;
    SE_G::IconPass* m_iconPass;

    float m_deltaTime = 0.0f;

private:
    //eastl::shared_ptr<PhysicsSystem> m_physicsSystem;
    // testing
    // SE::UUID floorId;
    // SE::UUID ballId;
};
