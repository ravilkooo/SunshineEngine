#pragma once
//#include <GraphicsUtils/SceneNode.h>
#include <GameObject.h>

class TestCube :
    public GameObject
{
public:
    TestCube();
    TestCube(ID3D11Device* device);
    TestCube(ID3D11Device* device, float width, float height, float depth, Vector3 position, Vector4 col);

    Vector3 position = Vector3::Zero;

    void SetInitTransform(Matrix transformMat);
    float speed = 0.0f;
    float angle = 0.0f;

private:
    Bind::VertexShader* vertexShaderB = nullptr;
    Matrix initTransform = Matrix::Identity;
};

