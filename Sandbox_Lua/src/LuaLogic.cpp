#include "LuaLogic.h"
#include <iostream>
#include <cstring>

LuaLogic::LuaLogic()
    : L_(nullptr), scriptLoaded_(false), foundFunction_(false) {
}

LuaLogic::~LuaLogic() {
    Cleanup();
}

bool LuaLogic::Init(const std::string& scriptPath) {
    Cleanup();

    L_ = luaL_newstate();
    luaL_openlibs(L_);

    if (luaL_dofile(L_, scriptPath.c_str()) != LUA_OK) {
        errorMessage_ = "Error running Lua script: " + std::string(lua_tostring(L_, -1));
        lua_pop(L_, 1);
        return false;
    }

    scriptLoaded_ = true;
    return true;
}

void LuaLogic::Cleanup() {
    if (L_) {
        lua_close(L_);
        L_ = nullptr;
    }
    ClearState();
}

void LuaLogic::ClearState() {
    scriptLoaded_ = false;
    foundFunction_ = false;
    params_.clear();
    lastResult_.clear();
    errorMessage_.clear();
    functionName_.clear();
}

void LuaLogic::LoadScript(const std::string& path) {
    if (!Init(path)) {
        // Init sets errorMessage_ in case of failure
        foundFunction_ = false;
        params_.clear();
        lastResult_.clear();
    }
}

void LuaLogic::SetFunctionName(const std::string& name) {
    functionName_ = name;
}

std::string LuaLogic::GetFunctionName() const {
    return functionName_;
}

bool LuaLogic::FindFunction(const std::string& funcName) {
    if (!scriptLoaded_ || funcName.empty()) {
        errorMessage_ = "Lua not initialized or function name empty!";
        return false;
    }

    functionName_ = funcName;
    foundFunction_ = false;
    errorMessage_.clear();
    params_.clear();

    lua_getglobal(L_, funcName.c_str());
    if (lua_isfunction(L_, -1)) {
        foundFunction_ = true;
        lua_pop(L_, 1);

        LoadParamsFromLua();

        return true;
    }
    else {
        lua_pop(L_, 1);
        errorMessage_ = "No such function: " + funcName;
        return false;
    }
}

void LuaLogic::LoadParamsFromLua() {
    lua_getglobal(L_, (functionName_ + "_params").c_str());
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

            params_.push_back(entry);

            lua_pop(L_, 1); // pop param entry
        }
    }
    lua_pop(L_, 1); // pop _params table
}

bool LuaLogic::CallFunction() {
    if (!foundFunction_ || functionName_.empty()) {
        errorMessage_ = "No function loaded to call";
        return false;
    }

    lua_getglobal(L_, functionName_.c_str());
    for (const auto& p : params_) {
        if (p.type == "number")
            lua_pushnumber(L_, atof(p.value.data()));
        else if (p.type == "bool")
            lua_pushboolean(L_, std::string(p.value.data()) == "true" || std::string(p.value.data()) == "1");
        else
            lua_pushstring(L_, p.value.data());
    }

    if (lua_pcall(L_, params_.size(), 1, 0) != LUA_OK) {
        errorMessage_ = "Lua error: " + std::string(lua_tostring(L_, -1));
        lua_pop(L_, 1);
        lastResult_.clear();
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

    lastResult_ = buf;
    lua_pop(L_, 1);
    return true;
}
