#include <Scripting/LuaManager.h>

#include <Scene.h>
#include <GameObject/GameObject.h>
#include <Component/LuaComponent.h>

void LuaManager::InitializeBehavior()
{
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
