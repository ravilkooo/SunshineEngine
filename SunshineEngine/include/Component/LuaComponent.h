#pragma once
#include "Component.h"
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/memory.h>
#include "sol/sol.hpp"
#include "ScriptComponent.h"

class GameObject;

struct ParamEntry {
    eastl::string name;
    eastl::string type;
    char value[128] = {};
};

class LuaComponent : public Component
{
    friend class LuaComponent_Info;
public:
    LuaComponent();
    ~LuaComponent();

    LuaComponent(const LuaComponent&) = delete;
    LuaComponent& operator=(const LuaComponent&) = delete;

    LuaComponent(LuaComponent&&) noexcept = default;
    LuaComponent& operator=(LuaComponent&&) noexcept = default;

    void Init(GameObject* owner, const eastl::string& inScriptPath);
    void Cleanup();

    void LoadScript();
    bool FindFunction();
    bool CallFunction();

    bool IsScriptLoaded() const { return scriptLoaded; }
    bool IsFunctionFound() const { return foundFunction; }
    eastl::string GetLastResult() const { return lastResult; }
    const eastl::vector<ParamEntry>& GetParams() const { return params; }
    eastl::vector<ParamEntry>& GetParams() { return params; }

    eastl::vector<eastl::string> GetAvailableFunctions() const;
    
    void SetFunctionName(const eastl::string& name);
    eastl::string GetFunctionName() const;

    //runtime
    void FromJson(const json& j, GameObject* obj);
    void LuaUpdate(float deltaTime);

    eastl::string scriptPath;
    eastl::string assetsPath;
    bool scriptLoaded;
    char functionName[128] = "";
    bool foundFunction;
    eastl::string lastResult;
    eastl::vector<ParamEntry> params;

    const std::type_info& getType() const override {
        return typeid(LuaComponent);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::LUA;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

private:
    eastl::unique_ptr<sol::state> lua;
    GameObject* obj;

    ScriptComponent scriptComponent;
    bool behaviorInitialized;

    void registerComponents();
    void ClearState();
    void LoadParamsFromLua();

    //runtime
    void InitializeBehavior();
};

class LuaComponent_Info : public Component_Info {
public:
    static const SE::ComponentType s_componentType = SE::ComponentType::LUA;

    LuaComponent_Info() {};
    LuaComponent_Info(int indexSelectedLuaFile) {};
    ~LuaComponent_Info() {};

    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    const std::type_info& getType() const override {
        return typeid(LuaComponent_Info);
    }

    bool IsAssigned() override { return false; }

    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;

    void InitLuaFile();
    void ScanLuaFiles(const eastl::string& dirPath);

    eastl::vector<eastl::string> luaFiles;
    eastl::string scriptPath;
    bool scriptLoaded;
    int selectedLuaFile = 0;

};