

#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
    for (auto& gameObject : gameObjects)
    {
        //delete &gameObject;
    }
}

void Scene::AddGameObject(eastl::unique_ptr<GameObject> gameObject)
{
    const Sunshine::UUID id = gameObject->m_UUID;
    auto [it, inserted] = uuidToObjectMap.emplace(id, nullptr);
    if (!inserted)
    {
        printf("Duplicate UUID in Scene::AddGameObject");
        return;
    }
    it->second = std::move(gameObject);
    gameObjects.push_back(id);
}

GameObject* Scene::GetGameObjectByUUID(Sunshine::UUID uuid) const
{
    auto it = uuidToObjectMap.find(uuid);
    return (it != uuidToObjectMap.end()) ? it->second.get() : nullptr;
}

eastl::unique_ptr<GameObject> Scene::RemoveGameObjectByUUID(Sunshine::UUID uuid)
{
    auto it = uuidToObjectMap.find(uuid);
    if (it == uuidToObjectMap.end())
        return nullptr;

    eastl::unique_ptr<GameObject> out = std::move(it->second);
    uuidToObjectMap.erase(it);

    for (size_t i = 0; i < gameObjects.size(); ++i)
    {
        if (gameObjects[i] == uuid)
        {
            // not keeping order
            eastl::swap(gameObjects[i], gameObjects.back());
            gameObjects.pop_back();
            // keeping order
            //gameObjects.erase(gameObjects.begin() + i);
            break;
        }
    }
    return out;
}

/*
void Scene::RemoveGameObject(eastl::unique_ptr<GameObject> gameObject)
{
    gameObjects.erase(eastl::remove(gameObjects.begin(), gameObjects.end(), gameObject), gameObjects.end());
}
*/
