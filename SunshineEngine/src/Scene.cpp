

#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
    for (auto& gameObject : gameObjects)
    {
        //delete &gameObject;
    }
}

void Scene::AddGameObject(eastl::unique_ptr<GameObject> gameObject)
{
    gameObjects.push_back(eastl::move(gameObject));
}

void Scene::RemoveGameObject(eastl::unique_ptr<GameObject> gameObject)
{
    gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), gameObject), gameObjects.end());
}

void Scene::Update(float deltaTime)
{
    for (const auto& gameObject : gameObjects)
    {
        //gameObject->Update(deltaTime);
    }
}

void Scene::Draw()
{
    for (const auto& gameObject : gameObjects)
    {
        //node->Draw();
    }
}