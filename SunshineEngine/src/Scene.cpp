#include <Scene.h>
#include <SceneHierarchy.h>

DeletionQueue::DeletionQueue()
    : queue(), head(0), tail(0), count(0)
{
    queue.resize(DELETION_QUEUE_CAPACITY, SE::UUID(0u));
}

void DeletionQueue::QueueForDestruction(SE::UUID uuid) {
    queue[tail] = uuid;
    tail = (tail + 1) % queue.size();
    ++count;
}

void DeletionQueue::Flush() {
    for (size_t i = 0; i < count; ++i) {
        size_t idx = (head + i) % queue.size();
        Scene::GetInstance().RemoveGameObjectByUUID(queue[idx]);
    }
    head = tail;
    count = 0;
}

bool DeletionQueue::IsEmpty() const { return count == 0; }

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

    m_playerObjectUUID = SE::UUID(0u);
    m_playerObject = nullptr;

    m_objectDestructionQueue.Flush();
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

GameObject* Scene::GetGameObjectByUUIDhilo(SE::UUIDhilo uuidhilo) const
{
	return GetGameObjectByUUID(SE::UUID::FromHilo(uuidhilo));
}

eastl::unique_ptr<GameObject> Scene::RemoveGameObjectByUUID(SE::UUID uuid)
{
    auto it = uuidToObjectMap.find(uuid);
    if (it == uuidToObjectMap.end())
        return nullptr;

    // Collect all children recursively
    eastl::vector<SE::UUID> toRemove;
    eastl::vector<SE::UUID> stack;
    stack.push_back(uuid);
    
    while (!stack.empty())
    {
        SE::UUID current = stack.back();
        stack.pop_back();
        toRemove.push_back(current);
        
        // Find object and add its children to stack
        auto objIt = uuidToObjectMap.find(current);
        if (objIt != uuidToObjectMap.end() && objIt->second)
        {
            for (SE::UUID childUUID : objIt->second->m_children)
            {
                stack.push_back(childUUID);
            }
        }
    }
    
    // Store root object to return
    eastl::unique_ptr<GameObject> out = std::move(uuidToObjectMap[uuid]);
    
    // Remove all collected objects (including root)
    for (SE::UUID removeUUID : toRemove)
    {
        // Remove from map
        auto mapIt = uuidToObjectMap.find(removeUUID);
        if (mapIt != uuidToObjectMap.end())
        {
            uuidToObjectMap.erase(mapIt);
        }
        
        // Remove from gameObjects vector
        for (size_t i = 0; i < gameObjects.size(); ++i)
        {
            if (gameObjects[i] == removeUUID)
            {
                eastl::swap(gameObjects[i], gameObjects.back());
                gameObjects.pop_back();
                break;
            }
        }
    }
    
    return out;
}

eastl::unique_ptr<GameObject> Scene::RemoveGameObjectByUUID(SE::UUIDhilo uuidhilo)
{
	return RemoveGameObjectByUUID(SE::UUID::FromHilo(uuidhilo));
}

void Scene::QueueGameObjectForDestruction(SE::UUID uuid)
{
    m_objectDestructionQueue.QueueForDestruction(uuid);
}

void Scene::QueueGameObjectForDestruction(SE::UUIDhilo uuidhilo)
{
    QueueGameObjectForDestruction(SE::UUID::FromHilo(uuidhilo));
}

void Scene::FlushDestructionQueue()
{
    m_objectDestructionQueue.Flush();
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
    if (!gameObjects.empty())
    {
        for (size_t i = 0; i < gameObjects.size(); ++i)
        {
            SE::UUID uuid = gameObjects[i];

            auto it = uuidToObjectMap.find(uuid);

            if (it == uuidToObjectMap.end())
                continue;

            uuidToObjectMap.erase(it);
        }
    }
    gameObjects.clear();
    uuidToObjectMap.clear();

    m_sceneGraph->Clear();
    m_playerObject = SE::UUID(0u);
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

    // Collect all children recursively (Note: GameObject_Info doesn't have m_children, relies on SceneGraph)
    // For Scene_Info, we just remove the single object since hierarchy is managed by SceneGraph
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