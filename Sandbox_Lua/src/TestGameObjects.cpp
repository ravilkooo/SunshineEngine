#include "TestGameObjects.h"
#include <Windows/DisplayWindow.h>
#include <LuaLogic.h>
bool showExampleWindow = true;
LuaLogic luaLogic;
// Win32 message handler
LRESULT CALLBACK WndProcImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) {}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DisplayWindow::WndProc(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);

}

TestGameObjects::TestGameObjects()
{
	applicationName = L"TestGameObjects";
	hInstance = GetModuleHandle(nullptr);

	luaLogic.Init();

	winWidth = 1000;
	winHeight = 800;

	timer = GameTimer();

	scene = Scene();

	displayWindow = DisplayWindow(this, applicationName, hInstance,
		winWidth, winHeight, WndProcImGui);

	renderer = new DeferredRenderer(displayWindow.hWnd, winWidth, winHeight);

	// GPass
	{

		GPass* gPass = new GPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight, renderer->pGBuffer, renderer->GetMainCamera());

		renderer->AddPass(gPass);
	}
	{
		LightPass* gLightPass = new LightPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight, renderer->pGBuffer, renderer->GetMainCamera());

		renderer->AddPass(gLightPass);
	}
	// FinalPass
	{
		FinalPass* colorPass = new FinalPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight, renderer->pGBuffer, renderer->GetMainCamera());

		renderer->AddPass(colorPass);
	}
	//Matrix::CreateFromQuaternion(Quaternion::FromToRotation({ 0,1,0 }, { 0,0,1 }));
	
	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &TestGameObjects::HandleKeyDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &TestGameObjects::HandleMouseMove);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(displayWindow.hWnd);
	ImGui_ImplDX11_Init(renderer->GetDevice(), renderer->GetDeviceContext());

	// Show window
	ShowWindow(displayWindow.hWnd, SW_SHOWDEFAULT);
	UpdateWindow(displayWindow.hWnd);

	auto factory = GameObjectFactory();
	defaultGameObject = factory.CreateDefaultBoxObject(renderer->GetDevice());
	defaultGameObject->GetComponent<TransformComponent>()->m_position.x += 3;

	scene.AddGameObject(eastl::move(defaultGameObject));

	scene.AddGameObject(eastl::move(factory.CreateDefaultBoxObject(renderer->GetDevice())));
	scene.AddGameObject(eastl::move(factory.CreateDefaultSphereObject(renderer->GetDevice())));

	scene.gameObjects[2]->GetComponent<TransformComponent>()->m_position.x -= 3;

	scene.AddGameObject(eastl::move(factory.CreateDefaultBoxObject(renderer->GetDevice(), 20.0f, 20.0f)));
	scene.gameObjects[3]->GetComponent<TransformComponent>()->m_position.z += 10.0f;
	scene.gameObjects[3]->GetComponent<TransformComponent>()->m_rotation = DXSM::Vector3::One * DX::XM_PIDIV2 * 0.3;


	scene.AddGameObject(eastl::move(factory.CreateAmbientLightObject(
		renderer->GetDevice(), renderer->GetMainCamera(), { DXSM::Vector3::One * 0.5f, 1.0f })
	));
	scene.AddGameObject(eastl::move(factory.CreatePointLightObject(renderer->GetDevice(), renderer->GetMainCamera(),
		{
			DXSM::Vector3(0.0, 0.0, 0.9), 1.0f,
			DXSM::Vector3(0.0, 0.0, 0.9), 1.0f,
			DXSM::Vector3(-1.0, 0.0, -0.9), 20,
			DXSM::Vector3(0.1, 0.1, 0.1), 0
		}
	)));

	scene.AddGameObject(eastl::move(factory.CreateDirectionalLightObject(renderer->GetDevice(), renderer->GetMainCamera())));
	auto skyBox = factory.CreateSkyBox(
		renderer->GetDevice(), renderer->GetMainCamera(), { DXSM::Vector3::One, 0.0f }, L"Default"
		//renderer->GetDevice(), renderer->GetMainCamera(), { DXSM::Vector3(1, 1, 1), 0.0f }
	);
	scene.AddGameObject(eastl::move(skyBox));

	scene.AddGameObject(eastl::move(factory.CreateFinalPassQuad(renderer->GetDevice())));

	luaLogic.Cleanup();

}

TestGameObjects::~TestGameObjects()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void TestGameObjects::Update(float deltaTime)
{
	scene.gameObjects[0]->GetComponent<TransformComponent>()->m_localRotation.y += deltaTime;
	scene.gameObjects[1]->GetComponent<TransformComponent>()->m_localRotation.x += deltaTime;
	scene.gameObjects[2]->GetComponent<TransformComponent>()->m_localRotation.y += deltaTime;
}

void TestGameObjects::Render()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Example window
	ImGui::Text("Lua Script:");
	if (ImGui::BeginCombo("##LuaFile", luaLogic.luaFiles.empty() ? "" : luaLogic.luaFiles[luaLogic.selectedLuaFile].c_str())) {
		for (int i = 0; i < luaLogic.luaFiles.size(); ++i) {
			bool is_selected = (i == luaLogic.selectedLuaFile);
			if (ImGui::Selectable(luaLogic.luaFiles[i].c_str(), is_selected))
				luaLogic.selectedLuaFile = i;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	if (ImGui::Button("Load Script")) {
		luaLogic.LoadScript();
	}

	if (luaLogic.scriptLoaded)
	{
		ImGui::Text("Function Name:"); ImGui::SameLine();
		ImGui::InputText("##FunctionName", luaLogic.functionName, IM_ARRAYSIZE(luaLogic.functionName));
		ImGui::SameLine();
		if (ImGui::Button("Find")) {
			luaLogic.FindFunction();
		}

		if (luaLogic.foundFunction) {
			ImGui::Text("Parameters:");
			for (int i = 0; i < luaLogic.params.size(); ++i) {
				ImGui::Text("%s (%s) =", luaLogic.params[i].name.c_str(), luaLogic.params[i].type.c_str());
				ImGui::SameLine();
				ImGui::InputText(("##p" + std::to_string(i)).c_str(), luaLogic.params[i].value.data(), luaLogic.params[i].value.size());
			}

			if (ImGui::Button("Call")) {
				luaLogic.CallFunction();
			}
			if (!luaLogic.lastResult.empty()) {
				ImGui::Text("%s", luaLogic.lastResult.c_str());
			}

		}
		else if (!luaLogic.errorMessage.empty()) {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", luaLogic.errorMessage.c_str());
		}
	}



	if (ImGui::Button("Close")) {
		showExampleWindow = false;
		PostQuitMessage(0);
	}

	// Rendering
	ImGui::Render();

	// Passes
	for (int i = 0; i < renderer->passes.size() - 1; i++) {
		renderer->GetDeviceContext()->ClearState();
		RenderPass* pass = renderer->passes[i];
		pass->StartFrame();
		pass->Pass(scene);
		pass->EndFrame();
	}

	renderer->GetDeviceContext()->ClearState();
	RenderPass* pass = renderer->passes.back();
	pass->StartFrame();
	pass->Pass(scene);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	pass->EndFrame();

	renderer->PresentFrame();
}

void TestGameObjects::HandleKeyDown(Keys key) {
	if (key == Keys::W)
	{
		renderer->mainCamera->MoveForward(deltaTime * 10.0f);
	}
	if (key == Keys::S)
	{
		renderer->mainCamera->MoveBackward(deltaTime * 10.0f);
	}
	if (key == Keys::A)
	{
		renderer->mainCamera->MoveLeft(deltaTime * 10.0f);
	}
	if (key == Keys::D)
	{
		renderer->mainCamera->MoveRight(deltaTime * 10.0f);
	}
	if (key == Keys::Space)
	{
		renderer->mainCamera->MoveUp(deltaTime * 10.0f);
	}

	if (key == Keys::LeftShift)
	{
		renderer->mainCamera->MoveDown(deltaTime * 10.0f);
	}
}


void TestGameObjects::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	renderer->mainCamera->RotateYaw(deltaTime * args.Offset.x * 0.1);
	renderer->mainCamera->RotatePitch(-deltaTime * args.Offset.y * 0.1);
}
