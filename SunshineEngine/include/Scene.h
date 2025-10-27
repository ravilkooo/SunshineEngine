#pragma once

#include "GameObject.h"
#include "Utils/UUID.h"
#include <EASTL/hash_map.h>
#include <EASTL/unique_ptr.h>

#include <unordered_map>

class Scene
{
public:
    Scene();
    ~Scene();

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) noexcept = default;
    Scene& operator=(Scene&&) noexcept = default;

    void AddGameObject(eastl::unique_ptr<GameObject> gameObject);
    GameObject* GetGameObjectByUUID(Sunshine::UUID uuid) const;
    //void RemoveGameObject(eastl::unique_ptr<GameObject> gameObject);
    eastl::unique_ptr<GameObject> RemoveGameObjectByUUID(Sunshine::UUID uuid);

    // чтобы быстро итероваться последовательно
    eastl::vector<Sunshine::UUID> gameObjects;
    // владеет объектами. чтобы быстро находить по UUID
    std::unordered_map<Sunshine::UUID, eastl::unique_ptr<GameObject>> uuidToObjectMap;
private:
};
