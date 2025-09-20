#include "DeferredGame.h"
#include <Jolt/Jolt.h>
#include <VGJS.h>
#include <assimp/Importer.hpp>
#include <EASTL/allocator.h>

int main() {
	DeferredGame game = DeferredGame();
	game.Run();

	return 0;
}