#pragma once

#include <imgui.h>
#include <EASTL/string.h>

namespace EditorUI {
	// Small helper to centralize ImGui fonts used in the editor and
	// provide convenient Push/Pop wrappers for each style.
	struct FontStyles {
		enum class Style {
			Regular = 0,
			Title,
			Header1,
			Header2,
			Header3,
			Count
		};

		// Initialize fonts. Call once after ImGui context is created
		// and before you render UI. basePath may be empty (then filenames
		// are tried directly).
		static bool Init(ImGuiIO& io);

		// Retrieve font pointer for a style (may return nullptr if init failed)
		static ImFont* Get(Style s);

		// Push a named style font (calls ImGui::PushFont internally)
		static void Push(Style s);

		// Pop font pushed earlier (calls ImGui::PopFont)
		static void Pop();

		// Destroy/clear loaded font pointers (optional)
		static void Shutdown();

	private:
		static ImFont* s_fonts[(int)Style::Count];
	};
}


