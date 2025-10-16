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

	timer = GameTimer();

	scene = Scene();

	displayWindow = DisplayWindow(this, applicationName, hInstance,
		winWidth, winHeight, WndProcImGui);

	renderer = new DeferredRenderer(displayWindow.hWnd, winWidth, winHeight);

	// GPass
	GPass* gPass;
	{
		gPass = new GPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight);

		renderer->SetMainCamera(gPass->GetCamera());
		renderer->mainCamera->SetPosition({ 0, 0, -10 });

		renderer->AddPass(gPass);
	}
	{
		gLightPass = new LightPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight, gPass->pGBuffer, gPass->GetCamera());


		gLightPass->AddPerFrameBind(new Bind::Texture(renderer->GetDevice(), gPass->pGBuffer->pNormalSRV.Get(), 0u));
		gLightPass->AddPerFrameBind(new Bind::Texture(renderer->GetDevice(), gPass->pGBuffer->pAlbedoSRV.Get(), 1u));
		gLightPass->AddPerFrameBind(new Bind::Texture(renderer->GetDevice(), gPass->pGBuffer->pSpecularSRV.Get(), 2u));
		gLightPass->AddPerFrameBind(new Bind::Texture(renderer->GetDevice(), gPass->pGBuffer->pWorldPosSRV.Get(), 3u));

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

		gLightPass->AddPerFrameBind(new Bind::Sampler(renderer->GetDevice(), samplerDesc, 0u));

		renderer->AddPass(gLightPass);
	}
	// FinalPass
	{
		FinalPass* colorPass = new FinalPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight);

		colorPass->SetCamera(renderer->GetMainCamera());

		colorPass->AddPerFrameBind(new Bind::Texture(renderer->GetDevice(), gPass->pGBuffer->pLightSRV.Get(), 0u));

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

		colorPass->AddPerFrameBind(new Bind::Sampler(renderer->GetDevice(), samplerDesc, 0u));

		renderer->AddPass(colorPass);
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
		new ParticleSystem(renderer->GetDevice(), renderer->GetDeviceContext(), emitterDesc, simulatorDesc));
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
		new Bind::BlendState(renderer->GetDevice(), particleBlendDesc, particleBlendFactor, sampleMask));

	//new Bind::Texture(device, "bubbleBC7.dds", aiTextureType_DIFFUSE, 0u);
	
	gLightPass->particleSystems[0]->SetTexture(
		new Bind::Texture(renderer->GetDevice(),
		JoinWchar_Wstring(EDITOR_ASSETS_DIR, L"bubble24bpp.dds")));


	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &DeferredGame::HandleKeyDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &DeferredGame::HandleMouseMove);

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
	defaultGameObject = factory.CreateDefaultCubeObject(renderer->GetDevice());
	defaultGameObject->GetComponent<TransformComponent>()->m_position.x += 3;

	scene.AddGameObject(eastl::move(defaultGameObject));

	scene.AddGameObject(eastl::move(factory.CreateDefaultCubeObject(renderer->GetDevice())));
	scene.AddGameObject(eastl::move(factory.CreateDefaultSphereObject(renderer->GetDevice())));

	scene.gameObjects[2]->GetComponent<TransformComponent>()->m_position.x -= 3;

	scene.AddGameObject(eastl::move(factory.CreateAmbientLightObject(renderer->GetDevice(), renderer->GetMainCamera())));
	scene.AddGameObject(eastl::move(factory.CreatePointLightObject(renderer->GetDevice(), renderer->GetMainCamera(),
		{
			DXSM::Vector4(0.0, 0.0, 0.9, 1.0),
			DXSM::Vector4(0.0, 0.0, 0.9, 1.0),
			DXSM::Vector3(-1.0, 0.0, -0.9), 20,
			DXSM::Vector3(0.1, 0.1, 0.1), 0
		}
		)));

	scene.AddGameObject(eastl::move(factory.CreateFinalPassQuad(renderer->GetDevice())));

}

DeferredGame::~DeferredGame()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
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

	scene.gameObjects[0]->GetComponent<TransformComponent>()->m_localRotation.y += deltaTime;
	scene.gameObjects[1]->GetComponent<TransformComponent>()->m_localRotation.x += deltaTime;
	scene.gameObjects[2]->GetComponent<TransformComponent>()->m_localRotation.y += deltaTime;


	//renderer->mainCamera->RotateYaw(deltaTime);
	//renderer->mainCamera->MoveLeft(3*deltaTime);
}

void DeferredGame::Render()
{
	// Start the Dear ImGui frame
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

void DeferredGame::HandleKeyDown(Keys key) {
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
	renderer->mainCamera->RotateYaw(deltaTime * args.Offset.x * 0.1);
	renderer->mainCamera->RotatePitch(-deltaTime * args.Offset.y * 0.1);
}
