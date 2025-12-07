#pragma once

#include "GameObject/GameObject.h"
#include "Utils/UUID.h"
#include <EASTL/hash_map.h>
#include <EASTL/unique_ptr.h>

#include <unordered_map>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
class PhysicsSystem;
class SceneGraph;

class Scene
{
public:
    Scene();
    ~Scene();

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) noexcept = default;
    Scene& operator=(Scene&&) noexcept = default;

    void ClearScene();

    SE::UUID AddGameObject(eastl::unique_ptr<GameObject> gameObject);
    GameObject* GetGameObjectByUUID(SE::UUID uuid) const;
    //void RemoveGameObject(eastl::unique_ptr<GameObject> gameObject);
    eastl::unique_ptr<GameObject> RemoveGameObjectByUUID(SE::UUID uuid);

    // Чтобы быстро и последовательно итероваться
    eastl::vector<SE::UUID> gameObjects;
    // Владеет объектами. Нужен чтобы быстро находить по UUID
    std::unordered_map<SE::UUID, eastl::unique_ptr<GameObject>> uuidToObjectMap;

    static eastl::shared_ptr<Scene> FromJson(
        SE_G::DeferredRenderer* renderSystem,
        PhysicsSystem* m_physicsSystem,
        eastl::shared_ptr<SE_G::Camera> camera, const json& j);

    void RestoreParents();

private:
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
};