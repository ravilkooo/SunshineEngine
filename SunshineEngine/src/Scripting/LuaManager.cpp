#include <Scene.h>
#include <GameObject/GameObject.h>
#include <Component/LuaComponent.h>

#include <Scripting/LuaManager.h>
#include <Scripting/ComponentBindings.h>

#include <Utils/DebugUtils.h>

LuaManager::LuaManager()
{

}

void LuaManager::InitializeBehavior()
{
    luaState = sol::state();
    luaState.open_libraries(
        sol::lib::base,
        sol::lib::package,
        sol::lib::math,
        sol::lib::string,
        sol::lib::table,
        sol::lib::os
    );

    /*
    if (!luaState) {
        printSunshineErrorMessage("InitializeBehavior: lua is nullptr!");
        return;
    }
    */

    RegisterComponents();

    luaState["InputSystem"] = &PlayerInputSystem::GetInstance();

    for (SE::UUID objUUID : Scene::GetInstance().gameObjects) {
        auto obj = Scene::GetInstance().GetGameObjectByUUID(objUUID);
        if (obj->HasComponent<LuaComponent>()) {
            obj->GetComponent<LuaComponent>()->InitializeBehavior();
        }
    }
}

void LuaManager::Update(Scene* scene, float deltaTime) {
    for (SE::UUID objUUID : scene->gameObjects) {
        auto obj = scene->GetGameObjectByUUID(objUUID);
        if (!obj)
            continue;

        if (obj->HasComponent<LuaComponent>()) {
            obj->GetComponent<LuaComponent>()->LuaUpdate(deltaTime);
        }
    }
}

sol::table LuaManager::LoadScript(const AssetPath& scriptPath)
{
    auto result = luaState.script_file(WStringToUtf8(scriptPath.GetFullPath()).c_str());
    if (!result.valid())
    {
        sol::error err = result;
        printSunshineErrorMessage((eastl::string("Error loading Lua script: ") + err.what()));
    }
    return result;
}

void LuaManager::RegisterComponents()
{
    ScriptingBindings::RegisterAll(luaState);
}
