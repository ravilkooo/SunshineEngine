#pragma once



#include "SceneNode.h"

class FullScreenQuad :
    public SceneNode
{
public:
    FullScreenQuad();
    FullScreenQuad(ID3D11Device* device);

    void Update(float deltaTime) {};

    Vector3 GetCenterLocation() { return Vector3::Zero;  };
private:
    Bind::VertexShader* vertexShaderB = nullptr;
};

