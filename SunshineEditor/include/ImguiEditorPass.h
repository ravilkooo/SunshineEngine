#pragma once

// ThirdPartyLibs
#include <sol/sol.hpp>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>

#include <sol_ImGui.h>

// SunshineLibs
#include <Graphics/Renderer/Pass/RenderPass.h>
#include <Graphics/Renderer/GBuffer.h>

#include "UI/ContentBrowserPanel.h"
#include "UI/MainMenuBarPanel.h"
#include "UI/ToolbarPanel.h"
#include "UI/BottomBarPanel.h"
#include "UI/LogPanel.h"
#include "UI/PropertyPanel.h"
#include "UI/ProjectSelector.h"

class EditorApp;
class WorldEditor;
struct Selection;
struct SceneNode;
class SceneGraph;

class ImguiEditorPass :
    public SE_G::RenderPass
{
public:
    ImguiEditorPass(EditorApp* editorApp);

    void SetVieportGBuffer(SE_G::GBuffer* pGBuffer);

    void StartFrame() override;
    void Pass() override;
    void EndFrame() override;

    void RenderGameWorld();
    void ShowContentBrowser();
    void ShowProperties();
    void ShowBottomPanel();
    void ShowOutputLog();
    void LuaImgui(GameObject*);
    
    bool IsProjectSelectorVisible() const { return m_ProjectSelector.IsVisible(); }
    void SetProjectSelectorVisible() { m_ProjectSelected = false; m_ProjectSelector.ResetSelection(); m_ProjectSelector.SetVisible(true); }
    bool IsProjectSelected() const { return m_ProjectSelected; }
    SE::Project* GetSelectedProject() { return m_ProjectSelector.GetSelectedProject(); }
    void ResetProjectSelection() { m_ProjectSelected = false; m_ProjectSelector.Close(); }

    UINT m_editorAppWidth = 800;
    UINT m_editorAppHeight = 800;

    bool m_isLayoutInitialized = false;

    EditorApp* m_editorApp;
    SE_G::GBuffer* m_viewportGBuffer;
    ID3D11Texture2D* m_backBuffer;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    D3D11_VIEWPORT m_windowViewport;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencil;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDSV;
    
    ContentBrowserPanel m_ContentBrowserPanel;
    MainMenuBarPanel m_MainMenuBarPanel;
    ToolbarPanel m_ToolbarPanel;
    LogPanel m_EditorLogPanel = LogPanel{"Editor Output Log", LogManager::LogTarget::Editor};
    LogPanel m_GameLogPanel = LogPanel{"Game Output Log", LogManager::LogTarget::Game};
    BottomBarPanel m_BottomPanel;
    PropertyPanel m_PropertyPanel;
    SE::ProjectSelector m_ProjectSelector;
    bool m_ProjectSelected = false;

    using SceneType = SE::SceneType;

    bool m_ShowEditorLogPanel = false;
    bool m_ShowGameLogPanel = false;

    void PreResize();
    void OnResize(UINT resizeWidth, UINT resizeHeight, ID3D11Texture2D* backBuffer);

    ImVec2 m_lastGameViewportSize = ImVec2(0, 0);
    bool m_gameViewportJustResized = false;

    bool IsFocusedGameViewport = false;
    bool IsHoveredGameViewport = false;

    // track mouse clicks on world editor
    struct MouseScreenCoords {
        UINT x;
        UINT y;
    } m_mouseClickCoords = { 0u, 0u };

    void ShowSceneHierarchy();
    void DrawNode(SceneNode* node, Selection& sel);
    void DrawSceneGraph(SceneGraph* g, Selection& sel);
private:
    //int selectedIdx = -1;
    // bool objectSelected = false;
    // SE::UUID selectedUUID;
};

