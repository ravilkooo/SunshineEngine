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

#include <EditorApp.h>

int main() {
	std::cout << "Hello, friend!\n";

	EditorApp* editorApp = new EditorApp();
	editorApp->InitEditorApp(1000u, 800u);
	editorApp->Run();
	
}
