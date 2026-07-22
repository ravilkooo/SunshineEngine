#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/MiniViewRenderer.h>
#include <Graphics/Renderer/Pass/SelectionPass.h>
#include <Graphics/Renderer/GBuffer.h>

#include <EASTL/string.h>
#include <EASTL/priority_queue.h>

#include "assimp/SceneCombiner.h"

#include "ImguiEditorPass.h"
#include <EditorApp.h>
#include <WorldEditor.h>
#include <SceneHierarchy.h>

#include <Component/LuaComponent.h>
#include <Component/CameraComponent.h>

#include <Utils/DebugUtils.h>
#include <Utils/StringUtils.h>

#include <UI/FontStyles.h>
#include "UI/PropertyPanel.h"

#include <Game.h>

#include <sstream>

template <typename T>
std::string toString(const T& t)
{
	std::ostringstream ss;
	ss << t;
	return ss.str();
}

ImguiEditorPass::ImguiEditorPass(
	EditorApp* editorApp)
	: RenderPass("ImGuiEditorPass", editorApp->m_imguiRenderGroup.get())
{
	auto device = m_renderer->GetDevice();

	m_editorAppWidth = editorApp->m_winWidth;
	m_editorAppHeight = editorApp->m_winHeight;
	m_editorApp = editorApp;
	m_backBuffer = editorApp->m_renderingSystem->GetBackBuffer();

	// rtv
	HRESULT hr = device->CreateRenderTargetView(m_backBuffer, nullptr, m_renderTargetView.GetAddressOf());
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

	m_windowViewport = {};
	m_windowViewport.Width = static_cast<float>(m_editorAppWidth);
	m_windowViewport.Height = static_cast<float>(m_editorAppHeight);
	m_windowViewport.TopLeftX = 0;
	m_windowViewport.TopLeftY = 0;
	m_windowViewport.MinDepth = 0;
	m_windowViewport.MaxDepth = 1.0f;

	// selectedUUID = SE::UUID(0u);

	ImGuiIO& io = ImGui::GetIO();
	// Init fonts
	EditorUI::FontStyles::Init(io);
	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();

	m_ToolbarPanel.Init(m_editorApp);
	m_Gizmo.Init();
	// m_PropertyPanel.SetAudioEditor(m_AudioEditor);
}

void ImguiEditorPass::SetVieportGBuffer(
	SE_G::GBuffer* pGBuffer) {
	m_viewportGBuffer = pGBuffer;
	// Call m_viewportGBuffer->OnResize() ?
}

void ImguiEditorPass::StartFrame()
{
	if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"ImguiEditor Pass");

	auto context = m_renderer->GetDeviceContext();

	context->OMSetRenderTargets(1u, m_renderTargetView.GetAddressOf(), m_pDSV.Get());
	float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	context->ClearRenderTargetView(m_renderTargetView.Get(), color);
	context->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	context->RSSetViewports(1, &m_windowViewport);
}

void ImguiEditorPass::Pass()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// ImGuizmo::BeginFrame();
	
	if (!m_ProjectSelected)
	{
		m_ProjectSelector.SetWindowSize(ImVec2(m_editorAppWidth, m_editorAppHeight));
		if (m_ProjectSelector.Show())
		{
			m_ProjectSelected = true;
		}
		
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		return;
	}
	
	// Main Menu Bar
	m_MainMenuBarPanel.SetEditorApp(m_editorApp);
	m_MainMenuBarPanel.SetImguiEditorPass(this);
	m_MainMenuBarPanel.OnImGuiRender();
	if (!m_ProjectSelected)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		m_editorApp->m_worldEditor->m_selectionPass->m_selectedObjectUUID = SE::UUID(0u);
		return;
	}

	// Toolbar
	m_ToolbarPanel.OnImGuiRender(m_MainMenuBarPanel.GetHeight());
	
	float topOffset = m_MainMenuBarPanel.GetHeight() + m_ToolbarPanel.GetHeight();

	// Create DockSpace above main viewport
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + topOffset));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - topOffset - m_BottomPanel.GetHeight()));
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
	}

	ImGui::End();

	//Scene Hierarchy
	ImGui::Begin("Scene Hierarchy");
	ShowSceneHierarchy();
	ImGui::End();

	// Properties
	ShowProperties();

	// Content Browser
	ShowContentBrowser();

	// Bottom Bar Panel
	ShowBottomPanel();

	// Output Log 
	ShowOutputLog();
	
	// Main Game Viewport
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	// Optional: zero item spacing if you want widgets to butt together
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGuiWindowFlags vp_flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;
	
	ImGui::Begin("Main Game Viewport", nullptr, vp_flags);

	ImGuizmo::BeginFrame();

	IsFocusedGameViewport = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
	IsHoveredGameViewport = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);

	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();
	ImVec2 windowPos = ImGui::GetWindowPos();

	m_Gizmo.SetWorldEditor(m_editorApp->m_worldEditor);
	m_Gizmo.SetViewportRect(
		ImVec2(windowPos.x + vMin.x, windowPos.y + vMin.y),
		ImVec2(vMax.x - vMin.x, vMax.y - vMin.y)
	);

	auto selectedObj = m_editorApp->m_worldEditor->m_scene->GetGameObjectByUUID(
		m_editorApp->m_worldEditor->m_hierarchySelection.last_clicked
	);
	m_Gizmo.SetSelectedObject(selectedObj);

	bool isEditorMode = (m_editorApp->m_runtimeMode == EditorApp::RuntimeMode::WORLD_EDITOR_MODE);

	if (IsHoveredGameViewport && isEditorMode)
	{
		if (!ImGuizmo::IsOver() &&
			(ImGui::IsMouseClicked(ImGuiMouseButton_Left)
				|| ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
		{
			ImVec2 mousePosScreen = ImGui::GetMousePos();
			// ImVec2 WindowPos = ImGui::GetWindowPos();

			m_mouseClickCoords = {
				(UINT)(mousePosScreen.x - windowPos.x - vMin.x),
				(UINT)(mousePosScreen.y - windowPos.y - vMin.y)
			};
			/*
			m_worldEditor->DeprojectScreenToWorld(
				DXSM::Vector2(MouseScreenCoords.x, MouseScreenCoords.y),
				DXSM::Vector2(m_lastGameViewportSize.x, m_lastGameViewportSize.y)
			);
			*/

			auto pixelInfo = m_editorApp->m_worldEditor->GetPixelInfo(m_mouseClickCoords.x, m_mouseClickCoords.y);
			uint64_t uuid = (uint64_t)pixelInfo.hi << 32 | pixelInfo.lo;
			auto selectedUUID = SE::UUID(uuid);
			if (selectedUUID != SE::UUID(0u))
			{
				m_editorApp->m_worldEditor->m_hierarchySelection.SetSingle(selectedUUID);
				m_clickWorldPos = pixelInfo.worldPos;
			}
			else
			{
				// Calculate place in front of camera on some distance
				m_clickWorldPos = DXSM::Vector3::Zero;
			}
			m_editorApp->m_worldEditor->m_selectionPass->m_selectedObjectUUID = selectedUUID;
			m_PropertyPanel.s_meshEditor.m_editMesh = false;
			m_PropertyPanel.s_meshEditor.m_editTexture = false;
		}
	}

	//ImVec2 contentSize = ImGui::GetContentRegionAvail();
	ImVec2 contentSize = ImVec2(
		vMax.x - vMin.x,
		vMax.y - vMin.y
	);
	m_gameViewportJustResized = (contentSize.x != m_lastGameViewportSize.x) || (contentSize.y != m_lastGameViewportSize.y);
	if (m_gameViewportJustResized && contentSize.x > 0 && contentSize.y > 0)
	{
		if (isEditorMode) {
			m_editorApp->m_worldEditor->OnResize((UINT)contentSize.x, (UINT)contentSize.y);
		}
		else {
			m_editorApp->m_currentGame->OnResize((UINT)contentSize.x, (UINT)contentSize.y);
		}
	}
	m_lastGameViewportSize = contentSize;

	
	if (isEditorMode && selectedObj && m_editorApp->m_worldEditor->m_hierarchySelection.last_clicked != SE::UUID(0u))
	{
		m_Gizmo.Update();
	}
	RenderGameWorld();

	if (isEditorMode && ImGui::BeginPopupContextWindow("viewport_contextmenu"))
	{
		if (ImGui::BeginMenu("Add..."))
		{
			if (ImGui::BeginMenu("Shape"))
			{
				if (ImGui::MenuItem("Plane"))
				{
					m_editorApp->m_worldEditor->AddPlaneShape(m_clickWorldPos);
				}
				if (ImGui::MenuItem("Box"))
				{
					m_editorApp->m_worldEditor->AddBoxShape(m_clickWorldPos);
				}
				if (ImGui::MenuItem("Sphere"))
				{
					m_editorApp->m_worldEditor->AddSphereShape(m_clickWorldPos);
				}
				if (ImGui::MenuItem("Geosphere"))
				{
					m_editorApp->m_worldEditor->AddGeosphereShape(m_clickWorldPos);
				}
				if (ImGui::MenuItem("Cylinder"))
				{
					m_editorApp->m_worldEditor->AddCylinderShape(m_clickWorldPos);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Lightning"))
			{
				if (ImGui::MenuItem("SkyBox"))
				{
					m_editorApp->m_worldEditor->AddSkyBox(m_clickWorldPos);
				}
				if (ImGui::MenuItem("Ambient"))
				{
					m_editorApp->m_worldEditor->AddAmbientLight(m_clickWorldPos);
				}
				if (ImGui::MenuItem("Directional"))
				{
					m_editorApp->m_worldEditor->AddDirectionalLight(m_clickWorldPos);
				}
				if (ImGui::MenuItem("Point Light"))
				{
					m_editorApp->m_worldEditor->AddPointLight(m_clickWorldPos);
				}
				if (ImGui::MenuItem("Spot Light"))
				{
					m_editorApp->m_worldEditor->AddSpotLight(m_clickWorldPos);
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Custom Mesh"))
			{
				m_editorApp->m_worldEditor->AddCustomMesh(m_clickWorldPos);
			}

			ImGui::EndMenu();
		}
		auto selectedUUID = m_editorApp->m_worldEditor->m_selectionPass->m_selectedObjectUUID;
		if (selectedUUID != SE::UUID(0u))
		{
			if (ImGui::MenuItem("Duplicate"))
			{
				auto j = m_editorApp->m_worldEditor->m_scene->GetGameObjectByUUID(selectedUUID)->ToJson();

				j["m_UUID"] = (uint64_t)SE::UUID();

				eastl::unique_ptr<GameObject_Info> go = Scene_Info::JsonToGameObject_Info(
					m_editorApp->m_worldEditor->m_scene, m_editorApp->m_worldEditor->m_renderer.get(), j);

				if (go)
				{
					go->GetComponent<TransformComponent_Info>()->m_assignedComponent->SetPosition(
						go->GetComponent<TransformComponent_Info>()->m_assignedComponent->GetPosition() + DXSM::Vector3(1, 0, 0)
					);
					auto uuid = m_editorApp->m_worldEditor->m_scene->AddGameObject(std::move(go));
					m_editorApp->m_worldEditor->m_scene->m_sceneGraph->Add(uuid);

					selectedUUID = uuid;
					m_editorApp->m_worldEditor->m_hierarchySelection.SetSingle(selectedUUID);
					m_editorApp->m_worldEditor->m_selectionPass->m_selectedObjectUUID = selectedUUID;
					m_PropertyPanel.s_meshEditor.m_editMesh = false;
					m_PropertyPanel.s_meshEditor.m_editTexture = false;
				}
			}
			if (ImGui::MenuItem("Copy"))
			{
				m_editorApp->m_worldEditor->m_copiedObjUUID = selectedUUID;
				m_editorApp->m_worldEditor->m_copiedObjSerialized = m_editorApp->m_worldEditor->m_scene->GetGameObjectByUUID(selectedUUID)->ToJson();
			}
		}
		if (m_editorApp->m_worldEditor->m_copiedObjUUID != SE::UUID(0u))
		{
			if (ImGui::MenuItem("Paste"))
			{
				m_editorApp->m_worldEditor->m_copiedObjSerialized["m_UUID"] = (uint64_t)SE::UUID();

				eastl::unique_ptr<GameObject_Info> go = Scene_Info::JsonToGameObject_Info(
					m_editorApp->m_worldEditor->m_scene, m_editorApp->m_worldEditor->m_renderer.get(),
					m_editorApp->m_worldEditor->m_copiedObjSerialized);

				if (go)
				{
					go->GetComponent<TransformComponent_Info>()->m_assignedComponent->SetPosition(m_clickWorldPos);
					auto uuid = m_editorApp->m_worldEditor->m_scene->AddGameObject(std::move(go));
					m_editorApp->m_worldEditor->m_scene->m_sceneGraph->Add(uuid);

					selectedUUID = uuid;
					m_editorApp->m_worldEditor->m_hierarchySelection.SetSingle(selectedUUID);
					m_editorApp->m_worldEditor->m_selectionPass->m_selectedObjectUUID = selectedUUID;
					m_PropertyPanel.s_meshEditor.m_editMesh = false;
					m_PropertyPanel.s_meshEditor.m_editTexture = false;
				}
			}
		}

		ImGui::EndPopup();
	}
	
	if (isEditorMode && selectedObj && m_editorApp->m_worldEditor->m_hierarchySelection.last_clicked != SE::UUID(0u)
		&& m_editorApp->m_worldEditor->m_selectionPass->m_selectedObjectUUID != SE::UUID(0u))
	{
		m_Gizmo.Draw();
	}

	ImGui::End();
	ImGui::PopStyleVar(4);

	if (!m_isLayoutInitialized)
	{
		m_isLayoutInitialized = true;

		ImGui::FocusWindow(NULL);
	}

	// -----------------

	// Some Lua-scripts linked with ImGui
	// Lua-script...


	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImguiEditorPass::EndFrame()
{
	auto context = m_renderer->GetDeviceContext();

	ID3D11ShaderResourceView* nullSRVs[] = { nullptr };
	context->PSSetShaderResources(0, 1, nullSRVs);
	ID3D11RenderTargetView* nullRTVs[] = { nullptr };
	context->OMSetRenderTargets(1, nullRTVs, nullptr);

	if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();
}

void ImguiEditorPass::RenderGameWorld()
{
	ImVec2 avail = ImGui::GetContentRegionAvail();
	ImGui::Image((ImTextureID)m_viewportGBuffer->pLightSRV.Get(), avail);
	// ����� ����� ���������� ���� ������� ����������, ���� ��������
	//ImGui::Text("Game World Render Here");

	if (m_editorApp->m_worldEditor && m_editorApp->m_worldEditor->m_renderer->m_mainCamera)
	{
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		ImVec2 windowPos = ImGui::GetWindowPos();
		auto m_viewportPos = ImVec2(windowPos.x + vMin.x, windowPos.y + vMin.y);
		auto m_viewportSize = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

		float cameraMode_padding = 5.0f;

		ImVec2 cameraMode_windowPos = ImVec2(
			m_viewportPos.x, m_viewportPos.y
		);

		ImGui::SetNextWindowPos(cameraMode_windowPos, ImGuiCond_Always, ImVec2(0.0f, 0.0f));
		ImGuiWindowFlags cameraMode_window_flags =
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(cameraMode_padding, cameraMode_padding - 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(cameraMode_padding * 0.5f, cameraMode_padding * 0.5f));

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));

		ImGui::Begin("CameraMode Controls", nullptr, cameraMode_window_flags);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.4f, 0.8f));

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		{
			ImGui::Spacing();
			if (ImGui::RadioButton("Perspective", m_editorApp->m_worldEditor->m_renderer->m_mainCamera->IsPerspectiveCamera()))
				m_editorApp->m_worldEditor->m_renderer->m_mainCamera->SwitchProjection();
			ImGui::SameLine();
			if (ImGui::RadioButton("Orhtographic", !m_editorApp->m_worldEditor->m_renderer->m_mainCamera->IsPerspectiveCamera()))
				m_editorApp->m_worldEditor->m_renderer->m_mainCamera->SwitchProjection();
		}

		ImGui::PopStyleColor(1);

		ImGui::End();

		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(2);
	}
}

void ImguiEditorPass::ShowSceneHierarchy()
{
	DrawSceneGraph(m_editorApp->m_worldEditor->m_scene->m_sceneGraph.get(), m_editorApp->m_worldEditor->m_hierarchySelection);
}

void ImguiEditorPass::ShowContentBrowser()
{
	m_ContentBrowserPanel.OnImGuiRender();
}

void ImguiEditorPass::ShowProperties()
{
	ImGui::Begin("Properties");
	if (ImGui::BeginTabBar("PropsTabBar"))
	{
		if (ImGui::BeginTabItem("Object Properties"))
		{
			ShowGameObjectProperties();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Player Settings"))
		{
			m_editorApp->m_worldEditor->m_miniViewRenderer->Enable();

			ShowGameplayProperties();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Audio Manager"))
		{
			m_editorApp->m_worldEditor->m_miniViewRenderer->Disable();

			if (m_AudioEditor)
			{
				m_PropertyPanel.DrawAudioPanel();
			}
			else
			{
				ImGui::TextDisabled("Audio Editor system not connected");
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void ImguiEditorPass::ShowGameObjectProperties()
{
	m_PropertyPanel.SetWorldEditor(m_editorApp->m_worldEditor);
	m_PropertyPanel.SetSelectedUUID(m_editorApp->m_worldEditor->m_hierarchySelection.last_clicked);
	m_PropertyPanel.OnImGuiRender();
}

void ImguiEditorPass::ShowGameplayProperties()
{
	m_gameplaySettingPanel.OnImGuiRender(m_editorApp->m_worldEditor.get());
}

void ImguiEditorPass::ShowBottomPanel()
{
	m_BottomPanel.OnImGuiRender(&m_ShowEditorLogPanel, &m_ShowGameLogPanel);
}

void ImguiEditorPass::ShowOutputLog()
{
	m_EditorLogPanel.SetBottomOffset(m_BottomPanel.GetHeight());
	
	if (m_ShowEditorLogPanel)
	{
		m_EditorLogPanel.OnImguiRender(m_ShowEditorLogPanel);
	}

	m_GameLogPanel.SetBottomOffset(m_BottomPanel.GetHeight());
	
	if (m_ShowGameLogPanel)
	{
		m_GameLogPanel.OnImguiRender(m_ShowGameLogPanel);
	}
}
	
void ImguiEditorPass::PreResize()
{
	// release RTV/DSV
	m_renderTargetView.Reset();
	m_pDSV.Reset();
	m_pDepthStencil.Reset();

}

void ImguiEditorPass::OnResize(UINT resizeWidth, UINT resizeHeight, ID3D11Texture2D* backBuffer)
{
	auto device = m_renderer->GetDevice();

	m_editorAppWidth = resizeWidth;
	m_editorAppHeight = resizeHeight;
	m_backBuffer = backBuffer;

	HRESULT hr = device->CreateRenderTargetView(m_backBuffer, nullptr, m_renderTargetView.GetAddressOf());
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

	m_windowViewport = {};
	m_windowViewport.Width = static_cast<float>(m_editorAppWidth);
	m_windowViewport.Height = static_cast<float>(m_editorAppHeight);
	m_windowViewport.TopLeftX = 0;
	m_windowViewport.TopLeftY = 0;
	m_windowViewport.MinDepth = 0;
	m_windowViewport.MaxDepth = 1.0f;
}

void ImguiEditorPass::DrawNode(SE::UUID nodeUUID, Selection& sel) {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_SpanFullWidth
		| ImGuiTreeNodeFlags_DefaultOpen
		| ImGuiTreeNodeFlags_DrawLinesToNodes;

	auto node = m_editorApp->m_worldEditor->m_scene->m_sceneGraph->m_nodes[
		m_editorApp->m_worldEditor->m_scene->m_sceneGraph->m_byObjUUID[nodeUUID]
	];
	auto obj = m_editorApp->m_worldEditor->m_scene->GetGameObjectByUUID(nodeUUID);

	const bool is_leaf = node.children.empty();
	if (is_leaf)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	if (sel.Contains(nodeUUID))
		flags |= ImGuiTreeNodeFlags_Selected;

	ImGui::PushID(obj); // stable id (or use UUID string)
	//bool open = ImGui::TreeNodeEx((void*)node, flags, "%s", node->objUUID.ToString().c_str());
	bool open = ImGui::TreeNodeEx((void*)obj, flags, "%s",
		m_editorApp->m_worldEditor->m_scene->m_sceneGraph->m_uuidToObjectMap[nodeUUID]->m_name);

	// Selection handling: click label to select; arrow toggles open.
	if (ImGui::IsItemClicked()) {
		// To-do: uncomment when mutliple selection will be implemented
		/*
		if (ImGui::GetIO().KeyCtrl) sel.Toggle(node->objUUID);
		else 
		*/
		sel.SetSingle(nodeUUID);
		m_editorApp->m_worldEditor->m_selectionPass->m_selectedObjectUUID = nodeUUID;
	}

	ImGui::SameLine();
	ImGui::TextDisabled("(%s)", nodeUUID.ToString().c_str());

	if (!is_leaf && open) {
		for (auto child : node.children)
			DrawNode(child, sel);
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void ImguiEditorPass::DrawSceneGraph(SceneGraph* g, Selection& sel) {
	for (auto root : g->m_roots)
		DrawNode(root, sel);
}