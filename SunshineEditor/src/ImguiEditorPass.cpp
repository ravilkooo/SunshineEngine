#include "ImguiEditorPass.h"
#include "WorldEditor.h"


ImguiEditorPass::ImguiEditorPass(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	ID3D11Texture2D* backBuffer,
	UINT editorAppWidth, UINT editorAppHeight,
	eastl::shared_ptr<GBuffer> pGBuffer,
	eastl::shared_ptr<WorldEditor> worldEditor)
	: RenderPass("LightPass", device, context)
{
	m_GBuffer = pGBuffer;
	m_editorAppWidth = editorAppWidth;
	m_editorAppHeight = editorAppHeight;
	m_worldEditor = worldEditor;
	m_backBuffer = backBuffer;

	// rtv
	HRESULT hr = device->CreateRenderTargetView(m_backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Render Target View");

	// depth buffer
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = m_editorAppWidth;
	descDepth.Height = m_editorAppHeight;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	device->CreateTexture2D(&descDepth, nullptr, &m_pDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	device->CreateDepthStencilView(m_pDepthStencil.Get(), &descDSV, m_pDSV.GetAddressOf());

	m_viewport = {};
	m_viewport.Width = static_cast<float>(m_editorAppWidth);
	m_viewport.Height = static_cast<float>(m_editorAppHeight);
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 1.0f;
}

void ImguiEditorPass::StartFrame()
{
	context->OMSetRenderTargets(1u, m_renderTargetView.GetAddressOf(), m_pDSV.Get());
	float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	context->ClearRenderTargetView(m_renderTargetView.Get(), color);
	context->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	context->RSSetViewports(1, &m_viewport);
}

void ImguiEditorPass::Pass(const Scene& scene)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Create DockSpace above main viewport
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	// ----- Docking -------
	//ImGui::DockSpaceOverViewport(0u, ImGui::GetMainViewport());

	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("DockSpace Demo", nullptr, window_flags);
	ImGui::PopStyleVar(2);

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	if (!m_isLayoutInitialized)
	{
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

		ImGuiID dock_main_id = dockspace_id;
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

		ImGui::DockBuilderDockWindow("Main Game Viewport", dock_main_id);
		ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_id_left);
		ImGui::DockBuilderDockWindow("Properties", dock_id_right);
		ImGui::DockBuilderDockWindow("Content Browser", dock_id_down);

		ImGui::DockBuilderFinish(dockspace_id);

		m_isLayoutInitialized = true;
	}

	ImGui::End();

	ImGui::Begin("Scene Hierarchy");
	ShowSceneHierarchy();  // Scene Hierarchy
	ImGui::End();

	ImGui::Begin("Content Browser");
	ShowContentBrowser();  // Content Browser
	ImGui::End();

	// Properties
	ImGui::Begin("Properties");
	ShowProperties();
	ImGui::End();

	// Main Game Viewport
	ImGui::Begin("Main Game Viewport");

	IsFocusedGameViewport = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

	ImVec2 contentSize = ImGui::GetContentRegionAvail();
	m_gameViewportJustResized = (contentSize.x != m_lastGameViewportSize.x) || (contentSize.y != m_lastGameViewportSize.y);
	if (m_gameViewportJustResized && contentSize.x > 0 && contentSize.y > 0) {
		
		m_GBuffer->OnResize(GetDevice(), (UINT)contentSize.x, (UINT)contentSize.y);
		m_worldEditor->OnResize((UINT)contentSize.x, (UINT)contentSize.y);
		//ResizeGBuffer((UINT)contentSize.x, (UINT)contentSize.y); // Your resize call
	}
	m_lastGameViewportSize = contentSize;

	//Tut


	RenderGameWorld();

	ImGui::End();

	// -----------------

	// Some Lua-scripts linked with ImGui
	// Lua-script...


	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImguiEditorPass::EndFrame()
{
	ID3D11ShaderResourceView* nullSRVs[] = { nullptr };
	context->PSSetShaderResources(0, 1, nullSRVs);
	ID3D11RenderTargetView* nullRTVs[] = { nullptr };
	context->OMSetRenderTargets(1, nullRTVs, nullptr);
}

void ImguiEditorPass::RenderGameWorld()
{
	ImVec2 avail = ImGui::GetContentRegionAvail();
	ImGui::Image((ImTextureID)m_GBuffer->pLightSRV.Get(), avail);
	// Здесь нужно отобразить ваше игровое содержимое, пока заглушка
	//ImGui::Text("Game World Render Here");
}

void ImguiEditorPass::ShowSceneHierarchy()
{
	ImGui::Text("Scene Hierarchy");
	if (ImGui::TreeNode("Root"))
	{
		ImGui::BulletText("Object A");
		ImGui::BulletText("Object B");
		ImGui::TreePop();
	}
}

void ImguiEditorPass::ShowContentBrowser()
{
	ImGui::Text("Content Browser");
	ImGui::Button("Import Asset");
}

void ImguiEditorPass::ShowProperties()
{
	ImGui::Text("Properties");
	//ImGui::InputText("Name", nullptr, 0); // Пример
}

void ImguiEditorPass::PreResize()
{
	// release RTV/DSV
	m_renderTargetView.ReleaseAndGetAddressOf();
	m_pDSV.ReleaseAndGetAddressOf();
	m_pDepthStencil.ReleaseAndGetAddressOf();
	m_backBuffer.ReleaseAndGetAddressOf();

}

void ImguiEditorPass::OnResize(UINT resizeWidth, UINT resizeHeight, ID3D11Texture2D* backBuffer)
{
	m_editorAppWidth = resizeWidth;
	m_editorAppHeight = resizeHeight;
	m_backBuffer = backBuffer;

	HRESULT hr = device->CreateRenderTargetView(m_backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Render Target View");
	
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = m_editorAppWidth;
	descDepth.Height = m_editorAppHeight;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	device->CreateTexture2D(&descDepth, nullptr, m_pDepthStencil.GetAddressOf());

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	device->CreateDepthStencilView(m_pDepthStencil.Get(), &descDSV, m_pDSV.GetAddressOf());

	m_viewport = {};
	m_viewport.Width = static_cast<float>(m_editorAppWidth);
	m_viewport.Height = static_cast<float>(m_editorAppHeight);
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 1.0f;
}
