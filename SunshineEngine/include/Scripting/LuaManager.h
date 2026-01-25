#pragma once

class Scene;

class LuaManager {
public:
    void InitializeBehavior();
    void Update(Scene* scene, float deltaTime);
};
