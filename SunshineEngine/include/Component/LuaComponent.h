#pragma once
#include "Component.h"
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/memory.h>
#include <sol/sol.hpp>
#include <Utils/AssetPath.h>

class GameObject;

class LuaComponent : public Component
{
    friend class LuaComponent_Info;
    friend class LuaManager;
public:
    LuaComponent();
    ~LuaComponent();

    LuaComponent(const LuaComponent&) = delete;
    LuaComponent& operator=(const LuaComponent&) = delete;

    LuaComponent(LuaComponent&&) noexcept = default;
    LuaComponent& operator=(LuaComponent&&) noexcept = default;

    void Init(GameObject* owner, AssetPath inScriptPath);
    void Cleanup();

    void LoadScript();
    
    //runtime
    void FromJson(const json& j, GameObject* obj);
    void LuaUpdate(float deltaTime);

    AssetPath scriptPath;
    bool scriptLoaded;
    
    const std::type_info& getType() const override {
        return typeid(LuaComponent);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::LUA;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

private:
    GameObject* obj;

    struct
    {
        sol::table self;
        sol::protected_function start;
        sol::protected_function update;
        sol::protected_function destroy;
    } scriptData;

    bool behaviorInitialized;

    void ClearState();
    //void LoadParamsFromLua();

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

    static void ScanLuaFiles();
    static eastl::vector<AssetPath> luaFiles;

    void InitLuaFile();
    AssetPath scriptPath;
    bool scriptLoaded;

};