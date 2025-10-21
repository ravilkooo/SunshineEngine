#pragma once

#include <vector>
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

    // change to shared_ptr? to link to game objects
    eastl::vector <eastl::unique_ptr<GameObject>> gameObjects;
private:
};
