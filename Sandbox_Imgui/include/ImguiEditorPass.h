#pragma once

// ThirdPartyLibs
#include <sol/sol.hpp>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>

#include <sol_ImGui.h>

// SunshineLibs
#include <Graphics/RenderPass.h>
#include <Graphics/GBuffer.h>

class ImguiEditorPass :
    public RenderPass
{
public:
    ImguiEditorPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer,
        UINT screenWidth, UINT screenHeight, eastl::shared_ptr<GBuffer> pGBuffer);

    void StartFrame() override;
    void Pass(const Scene& scene) override;
    void EndFrame() override;

    void RenderGameWorld();
    void ShowSceneHierarchy();
    void ShowContentBrowser();
    void ShowProperties();

    UINT m_editorAppWidth = 800;
    UINT m_editorAppHeight = 800;

    bool m_isLayoutInitialized = false;

    eastl::shared_ptr<GBuffer> m_GBuffer;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;

    ID3D11RenderTargetView* m_renderTargetView;
    D3D11_VIEWPORT m_viewport;
    ID3D11Texture2D* m_pDepthStencil;
    ID3D11DepthStencilView* m_pDSV;
};

