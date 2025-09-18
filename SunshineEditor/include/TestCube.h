#pragma once
#include <GraphicsUtils/SceneNode.h>

class TestCube :
    public SceneNode
{
public:
    TestCube();
    TestCube(ID3D11Device* device);
    TestCube(ID3D11Device* device, float width, float height, float depth, Vector3 position, Vector4 col);

    void Update(float deltaTime) override;

    Vector3 GetCenterLocation() override;

    Vector3 position = Vector3::Zero;

    void SetInitTransform(Matrix transformMat);
    float speed = 0.0f;
    float angle = 0.0f;

private:
    Bind::VertexShader* vertexShaderB = nullptr;
    Matrix initTransform = Matrix::Identity;
};

