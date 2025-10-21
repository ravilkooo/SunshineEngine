#pragma once
#include "Component.h"
#include <string>
#include <vector>
#include <memory>
#include "sol/sol.hpp"
#include <GameObject.h>

struct ParamEntry {
    std::string name;
    std::string type;
    std::array<char, 128> value = {};
};

class LuaComponent : public Component
{
public:
    LuaComponent();
    ~LuaComponent();

    void Init(GameObject*);
    void Cleanup();

    void LoadScript();
    bool FindFunction();
    bool CallFunction();

    bool IsScriptLoaded() const { return scriptLoaded; }
    bool IsFunctionFound() const { return foundFunction; }
    std::string GetLastResult() const { return lastResult; }
    std::string GetErrorMessage() const { return errorMessage; }
    const std::vector<ParamEntry>& GetParams() const { return params; }
    std::vector<ParamEntry>& GetParams() { return params; }

    void SetFunctionName(const std::string& name);
    std::string GetFunctionName() const;

    std::string scriptPath;
    std::string assetsPath;
    int selectedLuaFile = 0;
    std::vector<std::string> luaFiles;
    bool scriptLoaded;
    char functionName[128] = "";
    bool foundFunction;
    std::string lastResult;
    std::string errorMessage;
    std::vector<ParamEntry> params;

    const std::type_info& getType() const override {
        return typeid(LuaComponent);
    }

private:
    std::unique_ptr<sol::state> lua;
    GameObject* obj;

    void InitLuaFile();
    void registerComponents();
    void ScanLuaFiles(const std::string& dirPath);
    void ClearState();
    void LoadParamsFromLua();
};

