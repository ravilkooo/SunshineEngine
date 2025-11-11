#pragma once
#include "Component.h"
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/memory.h>
#include "sol/sol.hpp"
#include <GameObject.h>
#include "ScriptComponent.h"

struct ParamEntry {
    eastl::string name;
    eastl::string type;
    char value[128] = {};
};

class LuaComponent : public Component
{
public:
    LuaComponent();
    ~LuaComponent();

    LuaComponent(const LuaComponent&) = delete;
    LuaComponent& operator=(const LuaComponent&) = delete;

    LuaComponent(LuaComponent&&) noexcept = default;
    LuaComponent& operator=(LuaComponent&&) noexcept = default;

    void Init(GameObject*);
    void Cleanup();

    void LoadScript();
    bool FindFunction();
    bool CallFunction();

    bool IsScriptLoaded() const { return scriptLoaded; }
    bool IsFunctionFound() const { return foundFunction; }
    eastl::string GetLastResult() const { return lastResult; }
    const eastl::vector<ParamEntry>& GetParams() const { return params; }
    eastl::vector<ParamEntry>& GetParams() { return params; }

    void SetFunctionName(const eastl::string& name);
    eastl::string GetFunctionName() const;

    //runtime
    void LuaUpdate(float deltaTime);

    eastl::string scriptPath;
    eastl::string assetsPath;
    int selectedLuaFile = 0;
    eastl::vector<eastl::string> luaFiles;
    bool scriptLoaded;
    char functionName[128] = "";
    bool foundFunction;
    eastl::string lastResult;
    eastl::vector<ParamEntry> params;

    const std::type_info& getType() const override {
        return typeid(LuaComponent);
    }

private:
    eastl::unique_ptr<sol::state> lua;
    GameObject* obj;

    ScriptComponent scriptComponent;
    bool behaviorInitialized;

    void InitLuaFile();
    void registerComponents();
    void ScanLuaFiles(const eastl::string& dirPath);
    void ClearState();
    void LoadParamsFromLua();

    //runtime
    void InitializeBehavior();
};

class LuaComponent_Info : public Component_Info {
public:
    static ComponentType StaticComponentType() {
        return ComponentType::SCRIPT;
    }

    const std::type_info& getType() const override {
        return typeid(LuaComponent_Info);
    }

    bool IsAssigned() override { return false; }

};