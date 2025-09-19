

#include "GraphicsUtils/Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
    for (auto node : nodes)
    {
        delete node;
    }
}

void Scene::AddNode(SceneNode* node)
{
    nodes.push_back(node);
}

void Scene::RemoveNode(SceneNode* node)
{
    nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
}

void Scene::Update(float deltaTime)
{
    for (auto node : nodes)
    {
        node->Update(deltaTime);
    }
}

void Scene::Draw()
{
    for (auto node : nodes)
    {
        //node->Draw();
    }
}