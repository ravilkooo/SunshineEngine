#include <Scripting/LuaManager.h>

#include <Scene.h>
#include <GameObject.h>
#include <Component/LuaComponent.h>

void LuaManager::Update(Scene& scene, float deltaTime) {
    for (SE::UUID objUUID : scene.gameObjects) {
        auto obj = scene.GetGameObjectByUUID(objUUID);
        if (obj->HasComponent<LuaComponent>()) {
            obj->GetComponent<LuaComponent>()->LuaUpdate(deltaTime);
        }
    }
}
