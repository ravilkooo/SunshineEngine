#pragma once

#include "GameObject.h"
#include "Utils/UUID.h"
#include <EASTL/hash_map.h>
#include <EASTL/unique_ptr.h>

#include <unordered_map>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Scene
{
public:
    Scene();
    ~Scene();

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) noexcept = default;
    Scene& operator=(Scene&&) noexcept = default;

    Sunshine::UUID AddGameObject(eastl::unique_ptr<GameObject> gameObject);
    GameObject* GetGameObjectByUUID(Sunshine::UUID uuid) const;
    //void RemoveGameObject(eastl::unique_ptr<GameObject> gameObject);
    eastl::unique_ptr<GameObject> RemoveGameObjectByUUID(Sunshine::UUID uuid);

    // ����� ������ ����������� ���������������
    eastl::vector<Sunshine::UUID> gameObjects;
    // ������� ���������. ����� ������ �������� �� UUID
    std::unordered_map<Sunshine::UUID, eastl::unique_ptr<GameObject>> uuidToObjectMap;
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

    Sunshine::UUID AddGameObject(eastl::unique_ptr<GameObject_Info> gameObject);
    GameObject_Info* GetGameObjectByUUID(Sunshine::UUID uuid) const;
    //void RemoveGameObject(eastl::unique_ptr<GameObject> gameObject);
    eastl::unique_ptr<GameObject_Info> RemoveGameObjectByUUID(Sunshine::UUID uuid);

    // Serialization
    json ToJson() const;
    static eastl::shared_ptr<Scene_Info> FromJson(const json& j);

    // ����� ������ ����������� ���������������
    eastl::vector<Sunshine::UUID> gameObjects;
    // ������� ���������. ����� ������ �������� �� UUID
    std::unordered_map<Sunshine::UUID, eastl::unique_ptr<GameObject_Info>> uuidToObjectMap;
};