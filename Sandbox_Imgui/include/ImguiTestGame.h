#pragma once
#include <iostream>

#include <sol/sol.hpp>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>

#include <sol_ImGui.h>

#include <ImguiTestGame.h>
#include <Graphics/DeferredRenderer.h>
#include <Graphics/GPass.h>
#include <Graphics/LightPass.h>
#include <Graphics/MainColorPass.h>

#include <GameObjectFactory.h>
#include <Game.h>

class ImguiTestGame : public Game
{
public:
	bool is_layout_initialized = false;

	~ImguiTestGame() {
		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	sol::state lua;
	void Update(float deltaTime) override;
	void Render() override;
	void InitGame();

	void RenderGameWorld();
	void ShowSceneHierarchy();
	void ShowContentBrowser();
	void ShowProperties();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resultSRV;
};
