#include "LuaLogic.h"
#include <iostream>
#include <cstring>
#include <filesystem>

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
    : lua(nullptr), scriptLoaded(false), foundFunction(false) {
}

LuaLogic::~LuaLogic() {
    Cleanup();
}

void LuaLogic::Init() {
    InitLuaFile();

    lua = std::make_unique<sol::state>();
    lua->open_libraries(sol::lib::base);


    auto result = lua->script_file(scriptPath);
    if (!result.valid()) {
        sol::error err = result;
        errorMessage = std::string("Error running Lua script: ") + err.what();
    }
    else {
        scriptLoaded = true;
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
    if (lua) {
        lua = nullptr;
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
    scriptLoaded = (lua != nullptr);
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

    //callSolFunction(functionName, 2, 3);

    foundFunction = false;
    errorMessage.clear();
    params.clear();

    sol::function func = (*lua)[functionName];
    if (func.valid()) {
        foundFunction = true;
        LoadParamsFromLua();
        return true;
    }
    else {
        errorMessage = "No such function";
        return false;
    }
}

void LuaLogic::LoadParamsFromLua() {
    params.clear();
    sol::object paramTable = (*lua)[(std::string(functionName) + "_params").c_str()];
    if (paramTable.valid() && paramTable.get_type() == sol::type::table) {
        sol::table tbl = paramTable;
        for (auto& pair : tbl) {
            ParamEntry entry;
            sol::table paramEntry = pair.second.as<sol::table>();
            entry.name = paramEntry["name"].get<std::string>();
            entry.type = paramEntry["type"].get<std::string>();
            params.push_back(entry);
        }
    }
}

bool LuaLogic::CallFunction() {
    sol::function func = (*lua)[functionName];
    if (!func.valid())
    {
        errorMessage = "No such function";
        return false;
    }

    std::vector<sol::object> args;
    for (const auto& p : params) {
        std::string val(p.value.data());
        if (p.type == "number") {
            args.push_back(sol::make_object(*lua, std::stod(val)));
        }
        else if (p.type == "bool") {
            args.push_back(sol::make_object(*lua, val == "true" || val == "1"));
        }
        else {
            args.push_back(sol::make_object(*lua, val));
        }
    }

    sol::protected_function_result result = func(sol::as_args(args));
    if (!result.valid()) {
        sol::error err = result;
        errorMessage = "Lua error: " + std::string(err.what());
        return false;
    }

    sol::object res = result.get<sol::object>();
    if (!res.valid() || res.is<sol::nil_t>()) lastResult = "";
    else if (res.is<std::string>()) lastResult = res.as<std::string>();
    else if (res.is<double>()) lastResult = std::to_string(res.as<double>());
    else if (res.is<bool>()) lastResult = res.as<bool>() ? "true" : "false";
    else lastResult = "<unsupported return type>";
    return true;
}
