#pragma once



#include <vector>
#include "SceneNode.h"

class Scene
{
public:
    Scene();
    ~Scene();

    void AddNode(SceneNode* node);
    void RemoveNode(SceneNode* node);
    void Update(float deltaTime);
    void Draw();

    std::vector<SceneNode*> nodes;
private:
};
