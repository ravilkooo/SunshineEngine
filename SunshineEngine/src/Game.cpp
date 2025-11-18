#include "Game.h"

Game::Game()
{
	//Initialize();
}

void Game::InitGame(
	eastl::shared_ptr<SE_G::DeferredRenderer> renderer,
	UINT screenWidth,
	UINT screenHeight)
{
	this->m_renderer = renderer;
	this->m_screenHeight = screenHeight;
	this->m_screenWidth = screenWidth;

	/*
	{
		m_gPass = eastl::make_shared<SE_G::GPass>(
			m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(m_gPass);
	}
	{
		m_lightPass = eastl::make_shared<SE_G::LightPass>(
			m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(m_lightPass);
	}
	*/
}

void Game::Run()
{
	/*
	float physicsUpdateFPS = 120.0f;
	float physicsUpdateMs = 1.0f / physicsUpdateFPS;
	float accumulator = 0.0f;
	float accumulatorLimit = 4.0f * physicsUpdateMs;

	MSG msg = {};
	bool isExitRequested = false;
	
	// FPS statitistic
	unsigned int frameCount = 0;
	float FPSstatisticTimer = 0;

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
		accumulator += m_deltaTime;
		accumulator = eastl::min(4.0f * physicsUpdateMs, accumulator);
	
		// FPS statistic
		FPSstatisticTimer += m_deltaTime;
		frameCount++;
		if (FPSstatisticTimer > 1.0f) {
			float fps = frameCount * 1.0f / FPSstatisticTimer;

			FPSstatisticTimer -= 1.0f;

			WCHAR text[256];
			swprintf_s(text, TEXT("FPS: %f"), fps);
			SetWindowText(m_displayWindow.m_hWnd, text);

			frameCount = 0;
		}


		while (accumulator >= physicsUpdateMs) {
			Update(physicsUpdateMs);
			accumulator -= physicsUpdateMs;
		}
		Render();
	}
	*/
}

void Update(float deltaTime) {

	// m_luaManager.Update(m_scene, deltaTime);
	// m_physicsSystem.Update(deltaTime);
}

void Game::Render()
{
	// m_renderer->RenderScene(m_scene);
}

void Game::OnResize(UINT resizeWidth, UINT resizeHeight) {
	//m_renderer->GetMainCamera()->SetUpCameraViewByAspectRatio(m_screenWidth * 1.0f / m_screenHeight);
	if (resizeHeight == m_screenHeight)
		m_renderer->GetMainCamera()->SetUpCameraViewByAspectRatio_horizontal(resizeWidth * 1.0f / resizeHeight);
	else if (resizeWidth == m_screenWidth)
		m_renderer->GetMainCamera()->SetUpCameraViewByAspectRatio_vertical(resizeWidth * 1.0f / resizeHeight);
	else
		m_renderer->GetMainCamera()->ResetCameraView(resizeWidth * 1.0f / resizeHeight);

	m_screenWidth = resizeWidth;
	m_screenHeight = resizeHeight;

	m_gPass->OnResize(resizeWidth, resizeHeight);
	m_lightPass->OnResize(resizeWidth, resizeHeight);
}

Game::~Game()
{
	// Освобождение ресурсов
}
