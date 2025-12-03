#include <Scene.h>
#include <SceneHierarchy.h>

Scene::Scene()
{
}

Scene::~Scene()
{
    ClearScene();
}

void Scene::ClearScene() {
    for (size_t i = 0; i < gameObjects.size(); ++i)
    {
        SE::UUID uuid = gameObjects[i];

        auto it = uuidToObjectMap.find(uuid);

        if (it == uuidToObjectMap.end())
            continue;

        uuidToObjectMap.erase(it);
    }
    gameObjects.clear();
}

SE::UUID Scene::AddGameObject(eastl::unique_ptr<GameObject> gameObject)
{
    const SE::UUID id = gameObject->m_UUID;
    auto [it, inserted] = uuidToObjectMap.emplace(id, nullptr);
    if (!inserted)
    {
        printf("Duplicate UUID in Scene::AddGameObject");
        return SE::UUID(0u);
    }
    it->second = std::move(gameObject);
    gameObjects.push_back(id);
    return id;
}

GameObject* Scene::GetGameObjectByUUID(SE::UUID uuid) const
{
    auto it = uuidToObjectMap.find(uuid);
    return (it != uuidToObjectMap.end()) ? it->second.get() : nullptr;
}

eastl::unique_ptr<GameObject> Scene::RemoveGameObjectByUUID(SE::UUID uuid)
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

void Scene::RestoreParents()
{
    for (auto& pair : uuidToObjectMap)
    {
        if (pair.second->m_parent.uuid != SE::UUID(0u))
        {
            ParentNode pn = pair.second->m_parent;
            pn.ptr = uuidToObjectMap[pair.second->m_parent.uuid].get();

            pair.second->SetParent(pn);
        }
    }
}

Scene_Info::Scene_Info()
{
}

Scene_Info::~Scene_Info()
{
    ClearScene();
}

void Scene_Info::ClearScene() {
    for (size_t i = 0; i < gameObjects.size(); ++i)
    {
        SE::UUID uuid = gameObjects[i];

        auto it = uuidToObjectMap.find(uuid);

        if (it == uuidToObjectMap.end())
            continue;

        uuidToObjectMap.erase(it);
    }
    gameObjects.clear();
}

SE::UUID Scene_Info::AddGameObject(eastl::unique_ptr<GameObject_Info> gameObject)
{
    const SE::UUID id = gameObject->m_UUID;
    auto [it, inserted] = uuidToObjectMap.emplace(id, nullptr);
    if (!inserted)
    {
        printf("Duplicate UUID in Scene_Info::AddGameObject");
        return SE::UUID(0u);
    }
    it->second = std::move(gameObject);
    gameObjects.push_back(id);
    return id;
}

GameObject_Info* Scene_Info::GetGameObjectByUUID(SE::UUID uuid) const
{
    auto it = uuidToObjectMap.find(uuid);
    return (it != uuidToObjectMap.end()) ? it->second.get() : nullptr;
}

eastl::unique_ptr<GameObject_Info> Scene_Info::RemoveGameObjectByUUID(SE::UUID uuid)
{
    auto it = uuidToObjectMap.find(uuid);
    if (it == uuidToObjectMap.end())
        return nullptr;

    eastl::unique_ptr<GameObject_Info> out = std::move(it->second);
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

void Scene_Info::RestoreParents()
{
    for (auto& pair : uuidToObjectMap)
    {
        if (pair.second->m_parent.uuid != SE::UUID(0u))
        {
            ParentNode pn = pair.second->m_parent;
            pn.ptr = uuidToObjectMap[pair.second->m_parent.uuid].get();

            pair.second->SetParent(pn);
        }
    }
}

void Scene_Info::InitHierarchy()
{
    m_sceneGraph = eastl::make_unique<SceneGraph>(uuidToObjectMap);
    m_sceneGraph->Build();
}