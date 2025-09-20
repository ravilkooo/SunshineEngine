#include "DeferredGame.h"
#include <Jolt/Jolt.h>
#include <VGJS.h>
#include <assimp/Importer.hpp>
#include <EASTL/allocator.h>
#include <imgui.h>

int main() {
	DeferredGame game = DeferredGame();
	game.Run();

	return 0;
}