#pragma once

#include "GameObject.h"

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
    void RemoveGameObject(eastl::unique_ptr<GameObject> gameObject);
    void Update(float deltaTime);
    void Draw();

    eastl::vector <eastl::unique_ptr<GameObject>> gameObjects;
private:
};
