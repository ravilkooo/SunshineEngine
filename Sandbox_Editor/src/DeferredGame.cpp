#include "DeferredGame.h"

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
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

DeferredGame::DeferredGame()
{
	applicationName = L"DeferredGame";
	hInstance = GetModuleHandle(nullptr);

	winWidth = 1000;
	winHeight = 800;

	m_timer = GameTimer();

	m_scene = Scene();

	physEngine = new PhysicsEngine(&m_scene);

	displayWindow = DisplayWindow(this, applicationName, hInstance,
		winWidth, winHeight, WndProcImGui);

	m_renderer = new DeferredRenderer(displayWindow.hWnd, winWidth, winHeight);

	// GPass
	GPass* gPass;
	{
		gPass = new GPass(m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->GetBackBuffer(), winWidth, winHeight);

		m_renderer->SetMainCamera(gPass->GetCamera());
		m_renderer->mainCamera->SetPosition({ 0, 0, -10 });

		m_renderer->AddPass(gPass);
	}
	{
		gLightPass = new LightPass(m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->GetBackBuffer(), winWidth, winHeight, gPass->pGBuffer, gPass->GetCamera());


		gLightPass->AddPerFrameBind(new Bind::Texture(m_renderer->GetDevice(), gPass->pGBuffer->pNormalSRV.Get(), 0u));
		gLightPass->AddPerFrameBind(new Bind::Texture(m_renderer->GetDevice(), gPass->pGBuffer->pAlbedoSRV.Get(), 1u));
		gLightPass->AddPerFrameBind(new Bind::Texture(m_renderer->GetDevice(), gPass->pGBuffer->pSpecularSRV.Get(), 2u));
		gLightPass->AddPerFrameBind(new Bind::Texture(m_renderer->GetDevice(), gPass->pGBuffer->pWorldPosSRV.Get(), 3u));

		// Usual sampler for all SRV
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		gLightPass->AddPerFrameBind(new Bind::Sampler(m_renderer->GetDevice(), samplerDesc, 0u));

		m_renderer->AddPass(gLightPass);
	}
	// FinalPass
	{
		FinalPass* colorPass = new FinalPass(m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->GetBackBuffer(), winWidth, winHeight);

		colorPass->SetCamera(m_renderer->GetMainCamera());

		colorPass->AddPerFrameBind(new Bind::Texture(m_renderer->GetDevice(), gPass->pGBuffer->pLightSRV.Get(), 0u));

		// Usual sampler for all SRV
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		colorPass->AddPerFrameBind(new Bind::Sampler(m_renderer->GetDevice(), samplerDesc, 0u));

		m_renderer->AddPass(colorPass);
	}

	TestCube* _tc = new TestCube(m_renderer->GetDevice(), 0.2, 0.2, 0.2, { 0,0,0 }, { 1,0,0,1 });
	_tc->SetInitTransform(Matrix::CreateFromYawPitchRoll(XM_PIDIV4, XM_PIDIV4, 0));
	m_scene.AddNode(_tc);

	TestCube* _tc_2 = new TestCube(m_renderer->GetDevice(), 0.2f, 0.2f, 0.2f, { 0.5f, 0.5f, -0.3f }, { 0.9f, 0.7f, 0.5f, 1.0f });
	_tc_2->SetInitTransform(Matrix::CreateFromYawPitchRoll(XM_PIDIV2 * 0.4, XM_PIDIV4 * 0.12, 0));
	_tc_2->speed = 3.0f;
	m_scene.AddNode(_tc_2);

	TestCube* _tc_3 = new TestCube(m_renderer->GetDevice(), 10.0f, 10.0f, 1.0f, { 0.0f, 0.0f, 3.0f }, { 1.0f, 0.7f, 0.5f, 1.0f });
	m_scene.AddNode(_tc_3);

	TestCube* _tc_4 = new TestCube(m_renderer->GetDevice(), 0.5f, 0.5f, 0.5f, { -1.0f, 1.0f, -0.6f }, { 0.1f, 0.7f, 0.9f, 1.0f });
	_tc_4->SetInitTransform(Matrix::CreateFromYawPitchRoll(XM_PIDIV2 * 0.3, XM_PIDIV4 * 0.52, 0));
	_tc_4->speed = 5.0f;
	m_scene.AddNode(_tc_4);

	PointLight* _pl_1 = new PointLight(m_renderer->GetDevice(), { -0.5f, 0.5f, -0.2f }, 3.0f, { 0.01f, 1.5f, 0.0f },
		{ 0.1, 0, 0, 1 }, { 1, 0, 0, 1 }, { 1, 0, 0, 1 });
	m_scene.AddNode(_pl_1);

	PointLight* _pl_2 = new PointLight(m_renderer->GetDevice(), { 1.0f, 0.5f, -0.4f }, 5.4f, { 0.01f, 1.0f, 0.0f },
		{ 0, 0.1, 0, 1 }, { 0, 1, 0, 1 }, { 0, 1, 0, 1 });
	m_scene.AddNode(_pl_2);

	_dl_1 = new DirectionalLight(m_renderer->GetDevice(), { 5.0f, 5.5f, -5.0f }, { -1.0f, -2.0f, 1.0f },
		{ 0.2f, 0.2f, 0.7f, 1 }, { 0.2f, 0.2f, 0.7f, 1 }, { 0.2f, 0.2f, 0.7f, 1 });
	m_scene.AddNode(_dl_1);

	AmbientLight* _al_1 = new AmbientLight(m_renderer->GetDevice(), { 0.1f, 0.1f, 0.1f, 1.0f });
	m_scene.AddNode(_al_1);

	_sl_1 = new SpotLight(m_renderer->GetDevice(), { -2.0f, 2.0, 6.0f }, 5.0f, { 0.0f, 0.0f, -1.0f },
		20, { 0.01f, 1.0f, 0.0f },
		{ 0, 0.1, 0, 1 }, { 0, 1, 0, 1 }, { 0, 1, 0, 1 });
	m_scene.AddNode(_sl_1);

	FullScreenQuad* fsq = new FullScreenQuad(m_renderer->GetDevice());
	m_scene.AddNode(fsq);

	for (SceneNode* node : m_scene.nodes) {
		node->camera = m_renderer->GetMainCamera();
	}

	//Matrix::CreateFromQuaternion(Quaternion::FromToRotation({ 0,1,0 }, { 0,0,1 }));
	Vector3 emitDir = { 0,0,1 };
	ParticleSystem::EmitterPointConstantBuffer emitterDesc =
	{
		Matrix::CreateFromQuaternion(Quaternion::FromToRotation({ 0,1,0 }, emitDir)),
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

	std::wstring ws = std::wstring(SANDBOX_EDITOR_ASSETS_DIR) + L"bubble24bpp.dds";

	gLightPass->particleSystems[0]->SetTexture(
		new Bind::Texture(m_renderer->GetDevice(), std::string(ws.begin(), ws.end()), 0u));



	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &DeferredGame::HandleKeyDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &DeferredGame::HandleMouseMove);

}

DeferredGame::~DeferredGame()
{
}

void DeferredGame::Update(float deltaTime)
{
	// particle test
	gLightPass->particleSystems[0]->SetEmitDir(_dl_1->directionalLightData.Direction);
	gLightPass->particleSystems[0]->Update(deltaTime);
	//gLightPass->accumulatedTime += deltaTime;
	

	//_sl_1->spotLightData.Direction = Vector3::Transform(_sl_1->spotLightData.Direction, Matrix::CreateRotationY(deltaTime));
	currTime += deltaTime;
	_sl_1->spotLightData.Spot = 15 + 10 * sin(10*currTime);
	_dl_1->directionalLightData.Direction = Vector3::Transform(_dl_1->directionalLightData.Direction, Matrix::CreateRotationY(5*deltaTime));
	physEngine->Update(deltaTime);

}

void DeferredGame::Run()
{

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(displayWindow.hWnd);
	ImGui_ImplDX11_Init(m_renderer->GetDevice(), m_renderer->GetDeviceContext());

	// Show window
	ShowWindow(displayWindow.hWnd, SW_SHOWDEFAULT);
	UpdateWindow(displayWindow.hWnd);

	MSG msg = {};
	ZeroMemory(&msg, sizeof(msg));

	bool isExitRequested = false;
	unsigned int frameCount = 0;
	float totalTime = 0;

	while (!isExitRequested) {
		// Handle the windows messages.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			// If windows signals to end the application then exit out.
			if (msg.message == WM_QUIT) {
				isExitRequested = true;
			}
			continue;
		}


		m_timer.Tick();
		m_deltaTime = m_timer.GetDeltaTime();
		totalTime += m_deltaTime;
		frameCount++;

		if (totalTime > 1.0f) {
			float fps = frameCount * 1.0f / totalTime;

			totalTime -= 1.0f;

			WCHAR text[256];
			swprintf_s(text, TEXT("FPS: %f"), fps);
			SetWindowText(displayWindow.hWnd, text);

			frameCount = 0;
		}

		Update(m_deltaTime);

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// Example window
		ImGui::Begin("Hello, ImGui!");
		ImGui::Text("This is a simple test.");
		ImGui::SliderFloat("Float value", &io.DeltaTime, 0.0f, 1.0f);
		if (ImGui::Button("Close"))
			PostQuitMessage(0);
		ImGui::End();

		Render();

	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void DeferredGame::Render()
{
	// Rendering
	ImGui::Render();
	//renderer->RenderScene(scene);
	
	// Passes
	//for (RenderPass* pass : renderer->passes) {
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
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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
