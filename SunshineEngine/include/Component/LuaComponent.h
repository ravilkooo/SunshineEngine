#pragma once
#include "Component.h"
#include <string>
#include <vector>
#include <memory>
#include "sol/sol.hpp"

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

    void Init();
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

private:
    std::unique_ptr<sol::state> lua;

    void InitLuaFile();
    void ScanLuaFiles(const std::string& dirPath);
    void ClearState();
    void LoadParamsFromLua();
};

