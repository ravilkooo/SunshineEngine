#pragma once
#include <sol/sol.hpp>

class Scene;

class LuaManager {
public:
    static LuaManager& GetInstance()
    {
        static LuaManager instance;
        return instance;
    }

    LuaManager(const LuaManager&) = delete;
    LuaManager& operator=(const LuaManager&) = delete;
    LuaManager(LuaManager&&) = delete;
    LuaManager& operator=(LuaManager&&) = delete;

    eastl::unique_ptr<sol::state> luaState = nullptr;

    void InitializeBehavior();
    void Update(Scene* scene, float deltaTime);

    sol::table LoadScript(const AssetPath& scriptPath);

	void RegisterComponents();
private:
    LuaManager();
};
