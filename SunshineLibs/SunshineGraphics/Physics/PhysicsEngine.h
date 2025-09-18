#pragma once



#include <vector>
#include "GraphicsUtils/Scene.h"

class PhysicsEngine
{
public:
    PhysicsEngine();
    PhysicsEngine(Scene* scene);
    ~PhysicsEngine();
    
    virtual void Update(float deltaTime);

    Scene* scene;
};
