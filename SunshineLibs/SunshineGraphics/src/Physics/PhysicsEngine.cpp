

#include "Physics/PhysicsEngine.h"

PhysicsEngine::PhysicsEngine()
{
}

PhysicsEngine::PhysicsEngine(Scene* scene) : scene(scene)
{
}

PhysicsEngine::~PhysicsEngine()
{
}

void PhysicsEngine::Update(float deltaTime)
{
    for (auto node : scene->nodes)
    {
        // Обновление физики для каждого узла
        node->Update(deltaTime);
    }
}