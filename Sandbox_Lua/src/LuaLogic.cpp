#include "LuaLogic.h"
#include <iostream>
#include <cstring>
#include <filesystem>
#include "sol/sol.hpp"

namespace fs = std::filesystem;

std::string wstringToString(const std::wstring& wideStr)
{
    return std::string(wideStr.begin(), wideStr.end());
}
void LuaLogic::ScanLuaFiles(const std::string& dirPath) {
    luaFiles.clear();
    for (auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".lua")
                luaFiles.push_back(filename);
        }
    }
}

LuaLogic::LuaLogic()
    : L_(nullptr), scriptLoaded(false), foundFunction(false) {
}

LuaLogic::~LuaLogic() {
    Cleanup();
}

void LuaLogic::Init() {
    InitLuaFile();

    L_ = luaL_newstate();
    luaL_openlibs(L_);

    //sol::state lua;
    //lua.open_libraries(lua);


    if (luaL_dofile(L_, scriptPath.c_str()) != LUA_OK) {
        errorMessage = "Error running Lua script: " + std::string(lua_tostring(L_, -1));
        lua_pop(L_, 1);
    }
}

void LuaLogic::InitLuaFile()
{
    assetsPath = wstringToString(SANDBOX_LUA_ASSETS_DIR) + "Scripts";
    ScanLuaFiles(assetsPath);
    if (!luaFiles.empty()) {
        scriptPath = assetsPath + "/" + luaFiles[selectedLuaFile];
    }
    std::cout << scriptPath << "\n";
}

void LuaLogic::Cleanup() {
    if (L_) {
        lua_close(L_);
        L_ = nullptr;
    }
    ClearState();
}

void LuaLogic::ClearState() {
    scriptLoaded = false;
    foundFunction = false;
    params.clear();
    lastResult.clear();
    errorMessage.clear();
}

void LuaLogic::LoadScript() {
    scriptPath = assetsPath + "/" + luaFiles[selectedLuaFile];
    Cleanup();
    Init();
    errorMessage.clear();
    foundFunction = false;
    params.clear();
    lastResult.clear();
    scriptLoaded = (L_ != nullptr);
}

void LuaLogic::SetFunctionName(const std::string& name) {
    //functionName = name;
}

std::string LuaLogic::GetFunctionName() const {
    return functionName;
}

bool LuaLogic::FindFunction() {
    if (!scriptLoaded) { //|| functionName.empty()
        errorMessage = "Lua not initialized or function name empty!";
        return false;
    }
    foundFunction = false;
    errorMessage.clear();
    params.clear();

    lua_getglobal(L_, functionName);
    if (lua_isfunction(L_, -1)) {
        foundFunction = true;
        lua_pop(L_, 1);

        LoadParamsFromLua();

        return true;
    }
    else {
        lua_pop(L_, 1);
        errorMessage = "No such function: " + std::string(functionName);
        return false;
    }
}

void LuaLogic::LoadParamsFromLua() {
    lua_getglobal(L_, (std::string(functionName) + "_params").c_str());
    if (lua_istable(L_, -1)) {
        int n = lua_rawlen(L_, -1);
        for (int i = 1; i <= n; ++i) {
            lua_rawgeti(L_, -1, i);

            ParamEntry entry;

            lua_getfield(L_, -1, "name");
            entry.name = lua_tostring(L_, -1);
            lua_pop(L_, 1);

            lua_getfield(L_, -1, "type");
            entry.type = lua_tostring(L_, -1);
            lua_pop(L_, 1);

            params.push_back(entry);

            lua_pop(L_, 1); // pop param entry
        }
    }
    lua_pop(L_, 1); // pop _params table
}

bool LuaLogic::CallFunction() {
    if (!foundFunction) { //|| functionName.empty()
        errorMessage = "No function loaded to call";
        return false;
    }

    lua_getglobal(L_, functionName);
    for (const auto& p : params) {
        if (p.type == "number")
            lua_pushnumber(L_, atof(p.value.data()));
        else if (p.type == "bool")
            lua_pushboolean(L_, std::string(p.value.data()) == "true" || std::string(p.value.data()) == "1");
        else
            lua_pushstring(L_, p.value.data());
    }

    if (lua_pcall(L_, params.size(), 1, 0) != LUA_OK) {
        errorMessage = "Lua error: " + std::string(lua_tostring(L_, -1));
        lua_pop(L_, 1);
        lastResult.clear();
        return false;
    }

    char buf[256];
    if (lua_isnumber(L_, -1))
        snprintf(buf, sizeof(buf), "Result: %f", lua_tonumber(L_, -1));
    else if (lua_isstring(L_, -1))
        snprintf(buf, sizeof(buf), "Result: %s", lua_tostring(L_, -1));
    else if (lua_isboolean(L_, -1))
        snprintf(buf, sizeof(buf), "Result: %s", lua_toboolean(L_, -1) ? "true" : "false");
    else
        snprintf(buf, sizeof(buf), "Result: <unknown type>");

    lastResult = buf;
    lua_pop(L_, 1);
    return true;
}
