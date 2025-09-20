#include "DeferredGame.h"
#include <Jolt/Jolt.h>
#include <VGJS.h>
#include <assimp/Importer.hpp>
#include <EASTL/allocator.h>
#include <imgui.h>

#include <lua.hpp>   // from your ThirdParty/Lua/src

int main() {
    lua_State* L = luaL_newstate();    // create new Lua state
    luaL_openlibs(L);                  // open standard libraries

    // run a Lua script file
    std::wstring script = ENGINE_ASSETS_DIR;
    script = script + L"Scripts/test.lua";

    if (luaL_dofile(L, std::string(script.begin(), script.end()).c_str()) != LUA_OK) {
        std::cerr << "Error running Lua script: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1); // remove error message
    }

    // call a Lua function from C++
    lua_getglobal(L, "add");  // push function onto stack
    lua_pushnumber(L, 5);     // push argument 1
    lua_pushnumber(L, 7);     // push argument 2

    if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
        std::cerr << "Error calling add(): " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
    else {
        double sum = lua_tonumber(L, -1);
        std::cout << "5 + 7 = " << sum << std::endl;
        lua_pop(L, 1); // remove return value
    }

    lua_close(L); // cleanup

	DeferredGame game = DeferredGame();
	game.Run();

	return 0;
}