#include <iostream>
#include <cstring>
#include <filesystem>
#include <Component/LuaComponent.h>
#include <Component/TransformComponent.h>

namespace fs = std::filesystem;

std::string wstringToString(const std::wstring& wideStr)
{
    return std::string(wideStr.begin(), wideStr.end());
}
void LuaComponent::ScanLuaFiles(const std::string& dirPath) {
    luaFiles.clear();
    for (auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".lua")
                luaFiles.push_back(filename);
        }
    }
}

LuaComponent::LuaComponent()
    : lua(nullptr), scriptLoaded(false), foundFunction(false) {
}

LuaComponent::~LuaComponent() {
    Cleanup();
}

void LuaComponent::Init(GameObject* obj) {

    this->obj = obj;

    InitLuaFile();

    lua = std::make_unique<sol::state>();
    lua->open_libraries(sol::lib::base);

    registerComponents();


    auto result = lua->script_file(scriptPath);
    if (!result.valid()) {
        sol::error err = result;
        errorMessage = std::string("Error running Lua script: ") + err.what();
    }
    else {
        scriptLoaded = true;
    }
}

void LuaComponent::registerComponents() 
{
    lua->new_usertype<DXSM::Vector3>("Vector3",
        "x", &DXSM::Vector3::x,
        "y", &DXSM::Vector3::y,
        "z", &DXSM::Vector3::z
    );

    lua->new_usertype<TransformComponent>("TransformComponent",
        "m_position", &TransformComponent::m_position
    );

    auto getTransform = [](GameObject* go) -> TransformComponent* {
        return go->GetComponent<TransformComponent>().get();
        };

    lua->new_usertype<GameObject>("GameObject",
        "getTransform", getTransform
    );
}

void LuaComponent::InitLuaFile()
{
    assetsPath = wstringToString(ENGINE_ASSETS_DIR) + "Scripts";
    ScanLuaFiles(assetsPath);
    if (!luaFiles.empty()) {
        scriptPath = assetsPath + "/" + luaFiles[selectedLuaFile];
    }
    std::cout << scriptPath << "\n";
}

void LuaComponent::Cleanup() {
    if (lua) {
        lua = nullptr;
    }
    ClearState();
}

void LuaComponent::ClearState() {
    scriptLoaded = false;
    foundFunction = false;
    params.clear();
    lastResult.clear();
    errorMessage.clear();
}

void LuaComponent::LoadScript() {
    scriptPath = assetsPath + "/" + luaFiles[selectedLuaFile];
    std::cout << scriptPath << " is loaded!" << std::endl;
    Cleanup();
    Init(obj);
    errorMessage.clear();
    foundFunction = false;
    params.clear();
    lastResult.clear();
    scriptLoaded = (lua != nullptr);
}

void LuaComponent::SetFunctionName(const std::string& name) {
    //functionName = name;
}

std::string LuaComponent::GetFunctionName() const {
    return functionName;
}

bool LuaComponent::FindFunction() {
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
        std::cout << errorMessage << std::endl;
        return false;
    }
}

void LuaComponent::LoadParamsFromLua() {
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

bool LuaComponent::CallFunction() {

    sol::function func = (*lua)[functionName];
    if (!func.valid())
    {
        errorMessage = "No such function";
        std::cout << errorMessage << std::endl;
        return false;
    }

    std::vector<sol::object> args;
    for (const auto& p : params) {
        std::string val(p.value.data());
        if (p.type == "userdata") {
            args.push_back(sol::make_object(*lua, obj));
        }
        else if (p.type == "number") {
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
        std::cout << errorMessage << std::endl;
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
