#include <iostream>

#include <sol/sol.hpp>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>

#include <sol_ImGui.h>

#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/Pass/GPass.h>
#include <Graphics/Renderer/Pass/LightPass.h>
#include <Graphics/Renderer/Pass/FinalPass.h>

#include <GameObjectFactory.h>

#include <EditorApp.h>

int main() {
	std::cout << "Hello, friend!\n";

	EditorApp* editorApp = new EditorApp();
	editorApp->InitEditorApp(1000u, 800u);
	editorApp->Run();
	
}
