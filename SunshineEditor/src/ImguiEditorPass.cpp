#include <EASTL/string.h>
#include <EASTL/priority_queue.h>

#include "assimp/SceneCombiner.h"

#include "ImguiEditorPass.h"
#include <EditorApp.h>
#include <WorldEditor.h>
#include <SceneHierarchy.h>

#include <Component/LuaComponent.h>
#include <Utils/DebugUtils.h>
#include <Utils/StringUtils.h>
#include <UI/FontStyles.h>

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
	: RenderPass("LightPass", editorApp->m_renderingSystem->GetDevice(),
		editorApp->m_renderingSystem->GetDeviceContext())
{
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
}

void ImguiEditorPass::SetVieportGBuffer(
	SE_G::GBuffer* pGBuffer) {
	m_viewportGBuffer = pGBuffer;
	// Call m_viewportGBuffer->OnResize() ?
}

void ImguiEditorPass::StartFrame()
{
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

	IsFocusedGameViewport = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
	IsHoveredGameViewport = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);

	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();

	if (IsHoveredGameViewport)
	{
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			ImVec2 MousePosScreen = ImGui::GetMousePos();
			ImVec2 WindowPos = ImGui::GetWindowPos();

			m_mouseClickCoords = {
				(UINT) (MousePosScreen.x - WindowPos.x - vMin.x),
				(UINT) (MousePosScreen.y - WindowPos.y - vMin.y)
			};
			/*
			m_worldEditor->DeprojectScreenToWorld(
				DXSM::Vector2(MouseScreenCoords.x, MouseScreenCoords.y),
				DXSM::Vector2(m_lastGameViewportSize.x, m_lastGameViewportSize.y)
			);
			*/

			if (m_editorApp->m_runtimeMode == EditorApp::RuntimeMode::WORLD_EDITOR_MODE) {
					auto selectedUUID = m_editorApp->m_worldEditor->ChooseObjectByClick(m_mouseClickCoords.x, m_mouseClickCoords.y);
				if (selectedUUID != SE::UUID(0u))
				{
					m_editorApp->m_worldEditor->m_hierarchySelection.SetSingle(selectedUUID);
				}
				m_editorApp->m_worldEditor->m_selectionPass->m_selectedObjectUUID = selectedUUID;
			}
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
		if (m_editorApp->m_runtimeMode == EditorApp::RuntimeMode::WORLD_EDITOR_MODE) {
			m_editorApp->m_worldEditor->OnResize((UINT)contentSize.x, (UINT)contentSize.y);
		}
		else {
			m_editorApp->m_currentGame->OnResize((UINT)contentSize.x, (UINT)contentSize.y);
		}
	}
	m_lastGameViewportSize = contentSize;

	RenderGameWorld();

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
	ID3D11ShaderResourceView* nullSRVs[] = { nullptr };
	context->PSSetShaderResources(0, 1, nullSRVs);
	ID3D11RenderTargetView* nullRTVs[] = { nullptr };
	context->OMSetRenderTargets(1, nullRTVs, nullptr);
}

void ImguiEditorPass::RenderGameWorld()
{
	ImVec2 avail = ImGui::GetContentRegionAvail();
	ImGui::Image((ImTextureID)m_viewportGBuffer->pLightSRV.Get(), avail);
	// ����� ����� ���������� ���� ������� ����������, ���� ��������
	//ImGui::Text("Game World Render Here");
}

void ImguiEditorPass::ShowSceneHierarchy()
{
	DrawSceneGraph(m_editorApp->m_worldEditor->m_scene->m_sceneGraph.get(), m_editorApp->m_worldEditor->m_hierarchySelection);
	/*
	InitHierarchy();

	ImGui::Text("Scene Hierarchy");
	if (ImGui::TreeNode(WStringToUtf8(m_editorApp->m_openedProject->GetSubPath()).c_str()))
	{
		auto& objects = m_editorApp->m_worldEditor->m_scene->gameObjects;
		for (size_t i = 0; i < objects.size(); ++i)
		{
			// selectedIdx
			ImGui::PushID((int)i);
			bool isSelected = (selectedUUID == objects[i].m_UUID);

			// �������� ���� �������� �� ������ � ���� �� �� ������� �� �������� �� ������
			// �������� ���� �������� �� �������

			//eastl::string objLabel = eastl::string("GameObject ") + to_string_eastl(i);
			//m_worldEditor->m_scene->GetGameObjectByUUID(objects[i])->Name = objLabel;

			eastl::string objName = m_editorApp->m_worldEditor->m_scene->GetGameObjectByUUID(objects[i])->m_name;
			if (objName == "")
				objName = std::to_string(objects[i].m_UUID).c_str();
			// if (ImGui::Selectable(std::to_string(objects[i].m_UUID).c_str(), isSelected))
			if (ImGui::Selectable(objName.c_str(), isSelected))
			{
				selectedUUID = objects[i];
				m_editorApp->m_worldEditor->m_selectionPass->m_selectedObjectUUID = selectedUUID;
			}
			ImGui::PopID();
		}

		for (auto& node : nodesHierarchy)
		{
			ShowNode(node);
		}

		ImGui::TreePop();
	}
	*/
}

void ImguiEditorPass::ShowContentBrowser()
{
	m_ContentBrowserPanel.OnImGuiRender();
}

void ImguiEditorPass::ShowProperties()
{
	m_PropertyPanel.SetWorldEditor(m_editorApp->m_worldEditor);
	m_PropertyPanel.SetSelectedUUID(m_editorApp->m_worldEditor->m_hierarchySelection.last_clicked);
	m_PropertyPanel.OnImGuiRender();

	GameObject_Info* obj = m_editorApp->m_worldEditor->m_scene->GetGameObjectByUUID(
		m_editorApp->m_worldEditor->m_hierarchySelection.last_clicked
	);

	
	//if (!obj->HasComponent<LuaComponent_Info>())
	//{
	//	if (ImGui::Button("Add Lua Script")) {
	//		obj->AddComponent<LuaComponent_Info>();
	//		auto lc_info = obj->GetComponent<LuaComponent_Info>();
	//		lc_info->selectedLuaFile = 5;
	//		lc_info->InitLuaFile();
	//	}
	//	return;	
	//}

	//if (ImGui::Button("Add Lua Script")) {
	//	obj->AddComponent<LuaComponent_Info>();
	//	auto lc_info = obj->GetComponent<LuaComponent_Info>();
	//	lc_info->selectedLuaFile = 2;
	//	lc_info->InitLuaFile();
	//}
	
	//ImGui::Begin("Properties);
	// if (selectedUUID == SE::UUID(0u))
	// 	return;
	//
	// GameObject* obj = m_worldEditor->m_scene.GetGameObjectByUUID(
	// 	selectedUUID
	// );
	//
	// if (!obj->HasComponent<LuaComponent>())
	// {
	// 	if (ImGui::Button("Add Lua Script")) {
	// 		obj->AddComponent<LuaComponent>();
	// 		auto lua2 = obj->GetComponent<LuaComponent>();
	// 		lua2->Init(obj);
	// 	}
	// 	return;
	// }
	// else 
	// {
	// 	LuaImgui(obj);
	// }
	//ImGui::End();
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
	
//void ImguiEditorPass::LuaImgui(GameObject* obj)
//{
//	eastl::shared_ptr<LuaComponent_Info> testComponent = obj->GetComponent<LuaComponent_Info>();
//
//	if (ImGui::BeginCombo("##LuaFile", testComponent->luaFiles.empty() ? "" : testComponent->luaFiles[testComponent->selectedLuaFile].c_str())) {
//		for (int i = 0; i < testComponent->luaFiles.size(); ++i) {
//			bool is_selected = (i == testComponent->selectedLuaFile);
//			if (ImGui::Selectable(testComponent->luaFiles[i].c_str(), is_selected))
//				testComponent->selectedLuaFile = i;
//			if (is_selected)
//				ImGui::SetItemDefaultFocus();
//		}
//		ImGui::EndCombo();
//	}
//	if (ImGui::Button("Load Script")) {
//		testComponent->LoadScript();
//	}
//
//	if (testComponent->scriptLoaded)
//	{
//		ImGui::Text("Function Name:"); ImGui::SameLine();
//		ImGui::InputText("##FunctionName", testComponent->functionName, IM_ARRAYSIZE(testComponent->functionName));
//		if (ImGui::Button("Find")) {
//			testComponent->FindFunction();
//		}
//
//		if (testComponent->foundFunction) {
//			ImGui::Text("Parameters:");
//
//			for (int i = 0; i < testComponent->params.size(); ++i) {
//				auto& param = testComponent->params[i];
//				ImGui::Text("%s (%s) =", param.name, param.type);
//				ImGui::SameLine();
//
//				if (!EASTLStringEqualsChar(param.type,"userdata")) {
//					ImGui::InputText(("##p" + to_string_eastl(i)).c_str(), param.value, sizeof(param.value));
//				}
//				else {
//					eastl::string objName = obj->m_name;
//					ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), objName.c_str());
//				}
//			}
//
//			if (ImGui::Button("Call")) {
//				testComponent->CallFunction();
//			}
//			if (!testComponent->lastResult.empty()) {
//				ImGui::Text("%s", testComponent->lastResult);
//			}
//
//		}
//		else if (!sunshineErrorMessage.empty()) {
//			ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", sunshineErrorMessage);
//		}
//	}
//}

void ImguiEditorPass::PreResize()
{
	// release RTV/DSV
	m_renderTargetView.Reset();
	m_pDSV.Reset();
	m_pDepthStencil.Reset();

}

void ImguiEditorPass::OnResize(UINT resizeWidth, UINT resizeHeight, ID3D11Texture2D* backBuffer)
{
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

void ImguiEditorPass::DrawNode(SceneNode* node, Selection& sel) {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_SpanFullWidth
		| ImGuiTreeNodeFlags_DefaultOpen
		| ImGuiTreeNodeFlags_DrawLinesToNodes;
	const bool is_leaf = node->children.empty();
	if (is_leaf)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	if (sel.Contains(node->objUUID))
		flags |= ImGuiTreeNodeFlags_Selected;

	ImGui::PushID(node); // stable id (or use UUID string)
	//bool open = ImGui::TreeNodeEx((void*)node, flags, "%s", node->objUUID.ToString().c_str());
	bool open = ImGui::TreeNodeEx((void*)node, flags, "%s",
		m_editorApp->m_worldEditor->m_scene->m_sceneGraph->m_uuidToObjectMap[node->objUUID]->m_name);

	// Selection handling: click label to select; arrow toggles open.
	if (ImGui::IsItemClicked()) {
		// To-do: uncomment when mutliple selection will be implemented
		/*
		if (ImGui::GetIO().KeyCtrl) sel.Toggle(node->objUUID);
		else 
		*/
		sel.SetSingle(node->objUUID);
		m_editorApp->m_worldEditor->m_selectionPass->m_selectedObjectUUID = node->objUUID;
	}

	if (!is_leaf && open) {
		for (auto* child : node->children)
			DrawNode(child, sel);
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void ImguiEditorPass::DrawSceneGraph(SceneGraph* g, Selection& sel) {
	for (auto* root : g->m_roots)
		DrawNode(root, sel);
}