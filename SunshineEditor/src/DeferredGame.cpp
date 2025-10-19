#include "DeferredGame.h"
#include <Windows/DisplayWindow.h>

// Win32 message handler
LRESULT CALLBACK WndProcImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
	// 	return true;

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

DeferredGame::DeferredGame()
{
	m_applicationName = L"DeferredGame";
	m_hInstance = GetModuleHandle(nullptr);

	m_winWidth = 1700;
	m_winHeight = 1000;

	m_timer = GameTimer();

	m_scene = Scene();

	m_displayWindow = DisplayWindow(this, m_applicationName, m_hInstance,
		m_winWidth, m_winHeight, WndProcImGui);

	m_renderer = eastl::make_unique<DeferredRenderer>(m_displayWindow.m_hWnd, m_winWidth, m_winHeight);
	m_renderer->InitGBuffer(m_winWidth, m_winHeight);

	// GPass
	{

		GPass* gPass = new GPass(m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->GetBackBuffer(), m_winWidth, m_winHeight, m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(gPass);
	}
	{
		gLightPass = new LightPass(m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->GetBackBuffer(), m_winWidth, m_winHeight, m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(gLightPass);
	}
	// FinalPass
	{
		FinalPass* colorPass = new FinalPass(m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->GetBackBuffer(), m_winWidth, m_winHeight, m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(colorPass);
	}
	/*
	TestCube* _tc = new TestCube(renderer->GetDevice(), 0.2, 0.2, 0.2, { 0,0,0 }, { 1,0,0,1 });
	_tc->SetInitTransform(Matrix::CreateFromYawPitchRoll(XM_PIDIV4, XM_PIDIV4, 0));
	scene.AddNode(_tc);

	TestCube* _tc_2 = new TestCube(renderer->GetDevice(), 0.2f, 0.2f, 0.2f, { 0.5f, 0.5f, -0.3f }, { 0.9f, 0.7f, 0.5f, 1.0f });
	_tc_2->SetInitTransform(Matrix::CreateFromYawPitchRoll(XM_PIDIV2 * 0.4, XM_PIDIV4 * 0.12, 0));
	_tc_2->speed = 3.0f;
	scene.AddNode(_tc_2);

	TestCube* _tc_3 = new TestCube(renderer->GetDevice(), 10.0f, 10.0f, 1.0f, { 0.0f, 0.0f, 3.0f }, { 1.0f, 0.7f, 0.5f, 1.0f });
	scene.AddNode(_tc_3);

	TestCube* _tc_4 = new TestCube(renderer->GetDevice(), 0.5f, 0.5f, 0.5f, { -1.0f, 1.0f, -0.6f }, { 0.1f, 0.7f, 0.9f, 1.0f });
	_tc_4->SetInitTransform(Matrix::CreateFromYawPitchRoll(XM_PIDIV2 * 0.3, XM_PIDIV4 * 0.52, 0));
	_tc_4->speed = 5.0f;
	scene.AddNode(_tc_4);

	PointLight* _pl_1 = new PointLight(renderer->GetDevice(), { -0.5f, 0.5f, -0.2f }, 3.0f, { 0.01f, 1.5f, 0.0f },
		{ 0.1, 0, 0, 1 }, { 1, 0, 0, 1 }, { 1, 0, 0, 1 });
	scene.AddNode(_pl_1);

	PointLight* _pl_2 = new PointLight(renderer->GetDevice(), { 1.0f, 0.5f, -0.4f }, 5.4f, { 0.01f, 1.0f, 0.0f },
		{ 0, 0.1, 0, 1 }, { 0, 1, 0, 1 }, { 0, 1, 0, 1 });
	scene.AddNode(_pl_2);

	_dl_1 = new DirectionalLight(renderer->GetDevice(), { 5.0f, 5.5f, -5.0f }, { -1.0f, -2.0f, 1.0f },
		{ 0.2f, 0.2f, 0.7f, 1 }, { 0.2f, 0.2f, 0.7f, 1 }, { 0.2f, 0.2f, 0.7f, 1 });
	scene.AddNode(_dl_1);

	AmbientLight* _al_1 = new AmbientLight(renderer->GetDevice(), { 0.1f, 0.1f, 0.1f, 1.0f });
	scene.AddNode(_al_1);

	_sl_1 = new SpotLight(renderer->GetDevice(), { -2.0f, 2.0, 6.0f }, 5.0f, { 0.0f, 0.0f, -1.0f },
		20, { 0.01f, 1.0f, 0.0f },
		{ 0, 0.1, 0, 1 }, { 0, 1, 0, 1 }, { 0, 1, 0, 1 });
	scene.AddNode(_sl_1);

	FullScreenQuad* fsq = new FullScreenQuad(renderer->GetDevice());
	scene.AddNode(fsq);

	for (SceneNode* node : scene.nodes) {
		node->camera = renderer->GetMainCamera();
	}
	*/

	//Matrix::CreateFromQuaternion(Quaternion::FromToRotation({ 0,1,0 }, { 0,0,1 }));
	DXSM::Vector3 emitDir = { 0,0,1 };
	ParticleSystem::EmitterPointConstantBuffer emitterDesc =
	{
		DXSM::Matrix::CreateFromQuaternion(DXSM::Quaternion::FromToRotation({ 0,1,0 }, emitDir)),
		{ 0, 0, 0, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 0 },
		100, 3, 5, 1,
		0.2, 0.2,
		0, 3.1415 * 2,
		3.1415 / 10, 0, 0, 0
	};
	ParticleSystem::SimulateParticlesConstantBuffer simulatorDesc = {
		{ 0, 1, 0, 0 }
	};
	gLightPass->particleSystems.push_back(
		new ParticleSystem(m_renderer->GetDevice(), m_renderer->GetDeviceContext(), emitterDesc, simulatorDesc));
	gLightPass->particleSystems[0]->camera = gLightPass->GetCamera();

	D3D11_BLEND_DESC particleBlendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
	particleBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	particleBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	particleBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	FLOAT* particleBlendFactor = NULL;
	UINT sampleMask = 0xffffffff;
	gLightPass->particleSystems[0]->SetBlendState(
		new Bind::BlendState(m_renderer->GetDevice(), particleBlendDesc, particleBlendFactor, sampleMask));

	//new Bind::Texture(device, "bubbleBC7.dds", aiTextureType_DIFFUSE, 0u);
	
	gLightPass->particleSystems[0]->SetTexture(
		new Bind::Texture(m_renderer->GetDevice(),
		JoinWchar_Wstring(EDITOR_ASSETS_DIR, L"bubble24bpp.dds")));


	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &DeferredGame::HandleKeyDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &DeferredGame::HandleMouseMove);
	/*
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(m_displayWindow.m_hWnd);
	ImGui_ImplDX11_Init(m_renderer->GetDevice(), m_renderer->GetDeviceContext());

	// Show window
	ShowWindow(m_displayWindow.m_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(m_displayWindow.m_hWnd);
	*/

	auto factory = GameObjectFactory();
	defaultGameObject = factory.CreateDefaultBoxObject(m_renderer->GetDevice());
	defaultGameObject->GetComponent<TransformComponent>()->m_position.x += 3;

	m_scene.AddGameObject(eastl::move(defaultGameObject));

	m_scene.AddGameObject(eastl::move(factory.CreateDefaultBoxObject(m_renderer->GetDevice())));
	m_scene.AddGameObject(eastl::move(factory.CreateDefaultSphereObject(m_renderer->GetDevice())));

	m_scene.gameObjects[2]->GetComponent<TransformComponent>()->m_position.x -= 3;

	m_scene.AddGameObject(eastl::move(factory.CreateDefaultBoxObject(m_renderer->GetDevice(), 20.0f, 20.0f)));
	m_scene.gameObjects[3]->GetComponent<TransformComponent>()->m_position.z += 10.0f;
	m_scene.gameObjects[3]->GetComponent<TransformComponent>()->m_rotation = DXSM::Vector3::One * DX::XM_PIDIV2*0.3;


	m_scene.AddGameObject(eastl::move(factory.CreateAmbientLightObject(
		m_renderer->GetDevice(), m_renderer->GetMainCamera(), { DXSM::Vector3::One * 0.5f, 1.0f })
	));
	m_scene.AddGameObject(eastl::move(factory.CreatePointLightObject(m_renderer->GetDevice(), m_renderer->GetMainCamera(),
		{
			DXSM::Vector3(0.0, 0.0, 0.9), 1.0f,
			DXSM::Vector3(0.0, 0.0, 0.9), 1.0f,
			DXSM::Vector3(-1.0, 0.0, -0.9), 20,
			DXSM::Vector3(0.1, 0.1, 0.1), 0
		}
		)));

	m_scene.AddGameObject(eastl::move(factory.CreateDirectionalLightObject(m_renderer->GetDevice(), m_renderer->GetMainCamera())));
	auto skyBox = factory.CreateSkyBox(
		m_renderer->GetDevice(), m_renderer->GetMainCamera(), { DXSM::Vector3::One, 0.0f }, L"Default"
		//renderer->GetDevice(), renderer->GetMainCamera(), { DXSM::Vector3(1, 1, 1), 0.0f }
	);
	m_scene.AddGameObject(eastl::move(skyBox));

	m_scene.AddGameObject(eastl::move(factory.CreateFinalPassQuad(m_renderer->GetDevice())));

}

DeferredGame::~DeferredGame()
{
	// Cleanup
	// ImGui_ImplDX11_Shutdown();
	// ImGui_ImplWin32_Shutdown();
	// ImGui::DestroyContext();
}

void DeferredGame::Update(float deltaTime) 
{
	/*
	// particle test
	gLightPass->particleSystems[0]->SetEmitDir(_dl_1->directionalLightData.Direction);
	gLightPass->particleSystems[0]->Update(deltaTime);
	//gLightPass->accumulatedTime += deltaTime;
	

	//_sl_1->spotLightData.Direction = Vector3::Transform(_sl_1->spotLightData.Direction, Matrix::CreateRotationY(deltaTime));
	currTime += deltaTime;
	_sl_1->spotLightData.Spot = 15 + 10 * sin(10*currTime);
	_dl_1->directionalLightData.Direction = Vector3::Transform(_dl_1->directionalLightData.Direction, Matrix::CreateRotationY(5*deltaTime));
	physEngine->Update(deltaTime);
	*/;

	m_scene.gameObjects[0]->GetComponent<TransformComponent>()->m_localRotation.y += deltaTime;
	m_scene.gameObjects[1]->GetComponent<TransformComponent>()->m_localRotation.x += deltaTime;
	m_scene.gameObjects[2]->GetComponent<TransformComponent>()->m_localRotation.y += deltaTime;


	//renderer->mainCamera->RotateYaw(deltaTime);
	//renderer->mainCamera->MoveLeft(3*deltaTime);
}

void DeferredGame::Render()
{
	// Start the Dear ImGui frame
	/*
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Example window
	ImGui::Begin("Hello, ImGui!");
	ImGui::Text("This is a simple test.");
	ImGui::SliderFloat("Float value", &(io->DeltaTime), 0.0f, 1.0f);
	if (ImGui::Button("Close"))
		PostQuitMessage(0);
	ImGui::End();

	// Rendering
	ImGui::Render();
	*/
	
	// Passes
	for (int i = 0; i < m_renderer->passes.size() - 1; i++) {
		m_renderer->GetDeviceContext()->ClearState();
		RenderPass* pass = m_renderer->passes[i];
		pass->StartFrame();
		pass->Pass(m_scene);
		pass->EndFrame();
	}

	m_renderer->GetDeviceContext()->ClearState();
	RenderPass* pass = m_renderer->passes.back();
	pass->StartFrame();
	pass->Pass(m_scene);
	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	pass->EndFrame();

	m_renderer->PresentFrame();
}

void DeferredGame::HandleKeyDown(Keys key) {
	if (key == Keys::W)
	{
		m_renderer->mainCamera->MoveForward(m_deltaTime * 10.0f);
	}
	if (key == Keys::S)
	{
		m_renderer->mainCamera->MoveBackward(m_deltaTime * 10.0f);
	}
	if (key == Keys::A)
	{
		m_renderer->mainCamera->MoveLeft(m_deltaTime * 10.0f);
	}
	if (key == Keys::D)
	{
		m_renderer->mainCamera->MoveRight(m_deltaTime * 10.0f);
	}
	if (key == Keys::Space)
	{
		m_renderer->mainCamera->MoveUp(m_deltaTime * 10.0f);
	}

	if (key == Keys::LeftShift)
	{
		m_renderer->mainCamera->MoveDown(m_deltaTime * 10.0f);
	}
	if (key == Keys::Q)
	{
		gLightPass->particleSystems[0]->DecrementEmissionRate(10);
	}
	if (key == Keys::E)
	{
		gLightPass->particleSystems[0]->IncrementEmissionRate(10);
	}
}


void DeferredGame::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	m_renderer->mainCamera->RotateYaw(m_deltaTime * args.Offset.x * 0.1);
	m_renderer->mainCamera->RotatePitch(-m_deltaTime * args.Offset.y * 0.1);
}
