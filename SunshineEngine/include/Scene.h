#pragma once

#define DELETION_QUEUE_CAPACITY 16384

#include "GameObject/GameObject.h"
#include "Utils/UUID.h"
#include <EASTL/hash_map.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/fixed_vector.h>

#include <unordered_map>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class PhysicsSystem;
class SceneGraph;
class PlayerObject;

class DeletionQueue {
    eastl::fixed_vector<SE::UUID, DELETION_QUEUE_CAPACITY> queue;
    size_t head = 0, tail = 0;
    size_t count = 0;

public:
    void QueueForDestruction(SE::UUID uuid);

    void Flush();

    bool IsEmpty() const;
};

class Scene
{
public:
    static Scene& GetInstance()
    {
        static Scene instance;
        return instance;
    }

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;

    ~Scene();

    void ClearScene();

    SE::UUID AddGameObject(eastl::unique_ptr<GameObject> gameObject);
    GameObject* GetGameObjectByUUID(SE::UUID uuid) const;
    GameObject* GetGameObjectByUUIDhilo(SE::UUIDhilo uuidhilo) const;
    //void RemoveGameObject(eastl::unique_ptr<GameObject> gameObject);
    void QueueGameObjectForDestruction(SE::UUID uuid);
    void QueueGameObjectForDestruction(SE::UUIDhilo uuidhilo);

    eastl::unique_ptr<GameObject> RemoveGameObjectByUUID(SE::UUID uuid);
    eastl::unique_ptr<GameObject> RemoveGameObjectByUUID(SE::UUIDhilo uuidhilo);

    // Чтобы быстро и последовательно итероваться
    eastl::vector<SE::UUID> gameObjects;
    // Владеет объектами. Нужен чтобы быстро находить по UUID
    std::unordered_map<SE::UUID, eastl::unique_ptr<GameObject>> uuidToObjectMap;

    static void FromJson(
        SE_G::DeferredRenderer* renderSystem,
        PhysicsSystem* m_physicsSystem,
        eastl::shared_ptr<SE_G::Camera> camera, const json& j);

    void RestoreParents();

    // PlayerObject
    SE::UUID m_playerObjectUUID = SE::UUID(0u);
    PlayerObject* m_playerObject = nullptr;

    DeletionQueue m_objectDestructionQueue;
    void FlushDestructionQueue();
private:
    Scene();
};

class Scene_Info {
public:

    Scene_Info();
    ~Scene_Info();

    Scene_Info(const Scene_Info&) = delete;
    Scene_Info& operator=(const Scene_Info&) = delete;
    Scene_Info(Scene_Info&&) noexcept = default;
    Scene_Info& operator=(Scene_Info&&) noexcept = default;

    void ClearScene();

    SE::UUID AddGameObject(eastl::unique_ptr<GameObject_Info> gameObject);
    GameObject_Info* GetGameObjectByUUID(SE::UUID uuid) const;
    //void RemoveGameObject(eastl::unique_ptr<GameObject> gameObject);
    eastl::unique_ptr<GameObject_Info> RemoveGameObjectByUUID(SE::UUID uuid);

    // Serialization
    json ToJson() const;
    static eastl::shared_ptr<Scene_Info> FromJson(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera, const json& j);

    void RestoreParents();

    // Чтобы быстро и последовательно итероваться
    eastl::vector<SE::UUID> gameObjects;
    // Владеет объектами. Нужен чтобы быстро находить по UUID
    std::unordered_map<SE::UUID, eastl::unique_ptr<GameObject_Info>> uuidToObjectMap;

    // Hierarchy
    eastl::unique_ptr<SceneGraph> m_sceneGraph;
    void InitHierarchy();

    // PlayerObject
    SE::UUID m_playerObject = SE::UUID(0u);
};