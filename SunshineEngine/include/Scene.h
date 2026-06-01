#pragma once

#define DELETION_QUEUE_CAPACITY 16384

#include "Utils/UUID.h"
#include <EASTL/hash_map.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/fixed_vector.h>

#include "InputSystem/PlayerInputSystem.h"

#include <unordered_map>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class PhysicsSystem;
class CameraManager;
class SceneGraph;

class GameObject;
class GameObject_Info;

namespace SE_G {
    class Camera;
    class DeferredRenderer;
}

class DeletionQueue {
    eastl::fixed_vector<SE::UUID, DELETION_QUEUE_CAPACITY> queue;
    size_t head = 0, tail = 0;
    size_t count = 0;

public:
    DeletionQueue();

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

    PhysicsSystem* m_physicsSystem = nullptr;

    DeletionQueue m_objectDestructionQueue;
    void FlushDestructionQueue();

    SE::UUID m_mainCameraUUID = SE::UUID(0u);
    eastl::unique_ptr<CameraManager> m_cameraManager;

    SE_G::DeferredRenderer* m_renderer;
    void SetRenderer(SE_G::DeferredRenderer* renderer);
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
    static eastl::unique_ptr<GameObject_Info> JsonToGameObject_Info(
        eastl::shared_ptr<Scene_Info> scene,
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera, const json& objJ);
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

    SE::UUID m_mainCameraUUID = SE::UUID(0u);
    eastl::unique_ptr<CameraManager> m_cameraManager;

    eastl::unique_ptr<PlayerInputSystem::KeyMapping_Info> m_keyMapping;
};