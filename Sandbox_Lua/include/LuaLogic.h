#pragma once
#include <string>
#include <vector>
#include <array>
#include <lua.hpp>

struct ParamEntry {
    std::string name;
    std::string type;
    std::array<char, 128> value = {};
};

class LuaLogic {
public:
    LuaLogic();
    ~LuaLogic();

    bool Init(const std::string& scriptPath);
    void Cleanup();

    void LoadScript(const std::string& path);
    bool FindFunction(const std::string& funcName);
    bool CallFunction();

    bool IsScriptLoaded() const { return scriptLoaded_; }
    bool IsFunctionFound() const { return foundFunction_; }
    std::string GetLastResult() const { return lastResult_; }
    std::string GetErrorMessage() const { return errorMessage_; }
    const std::vector<ParamEntry>& GetParams() const { return params_; }
    std::vector<ParamEntry>& GetParams() { return params_; }

    void SetFunctionName(const std::string& name);
    std::string GetFunctionName() const;

private:
    lua_State* L_;
    std::vector<ParamEntry> params_;
    std::string lastResult_;
    std::string errorMessage_;
    bool scriptLoaded_;
    bool foundFunction_;
    std::string functionName_;

    void ClearState();
    void LoadParamsFromLua();
};
