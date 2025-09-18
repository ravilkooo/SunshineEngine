#include "TestCube.h"

#include <Utils/ShapeGenerator.h>


TestCube::TestCube()
{
}

TestCube::TestCube(ID3D11Device* device) : TestCube(device, 0.2, 0.2, 0.2, { 0,0,0 }, { 1,0,0,1 })
{
}

TestCube::TestCube(ID3D11Device* device, float width, float height, float depth, Vector3 position, Vector4 col)
{
    //this->device = device;
    this->position = position;

    // DirectX::XMFLOAT4 col = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    CreateSimpleCubeMesh(width, height, depth, col, &vertices, &verticesNum, &indices, &indicesNum);

    auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);

    // GBufferPass
    {
        RenderTechnique* gBufferPass = new RenderTechnique("GBufferPass");
        gBufferPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        gBufferPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
        // AddStaticBind(texture);
        
        wchar_t filePath[100] = EDITOR_ASSETS_DIR;
        wcsncat(filePath, L"/Shaders/GBufferShaderVS.hlsl", 30);
        vertexShaderB = new Bind::VertexShader(device, filePath);
        gBufferPass->AddBind(vertexShaderB);


        numInputElements = 4;
        IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));

        IALayoutInputElements[0] =
            D3D11_INPUT_ELEMENT_DESC{
                "POSITION",
                0,
                DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                0,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };
        IALayoutInputElements[1] =
            D3D11_INPUT_ELEMENT_DESC{
                "COLOR",
                0,
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };
        IALayoutInputElements[2] =
            D3D11_INPUT_ELEMENT_DESC{
                "TEXCOORD",
                0,
                DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };
        IALayoutInputElements[3] =
            D3D11_INPUT_ELEMENT_DESC{
                "NORMAL",
                0,
                DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };

        gBufferPass->AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));

        wchar_t psFilePath[100] = EDITOR_ASSETS_DIR;
        wcsncat(psFilePath, L"/Shaders/GBufferShaderPS.hlsl", 30);
        gBufferPass->AddBind(new Bind::PixelShader(device, psFilePath));

        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        gBufferPass->AddBind(new Bind::Rasterizer(device, rastDesc));
        gBufferPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
        gBufferPass->AddBind(new Bind::TransformCBuffer(device, this, 0u));

        techniques.insert({ "GBufferPass", gBufferPass });
    }
    // MainColorPass
    /*
    {
        RenderTechnique* colorPass = new RenderTechnique("MainColorPass");
        colorPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        colorPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
        // AddStaticBind(texture);
        vertexShaderB = new Bind::VertexShader(device, L"./Assets/Shaders/CubeShaderVS.hlsl");
        colorPass->AddBind(vertexShaderB);


        numInputElements = 1;
        IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));

        IALayoutInputElements[0] =
            D3D11_INPUT_ELEMENT_DESC{
                "POSITION",
                0,
                DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                0,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };

        colorPass->AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));


        colorPass->AddBind(new Bind::PixelShader(device, L"./Assets/Shaders/CubeShaderPS.hlsl"));


        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        colorPass->AddBind(new Bind::Rasterizer(device, rastDesc));
        colorPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
        colorPass->AddBind(new Bind::TransformCBuffer(device, this, 0u));

        techniques.insert({ "MainColorPass", colorPass });
    }
    */

}


void TestCube::Update(float deltaTime)
{
    //Vector3 new_pos = position;
    // new_pos = Vector3::Transform(new_pos, Matrix::CreateRotationY(deltaTime * speed));
    //new_pos.y = position.y;
    //new_pos.z = position.z;
    angle += deltaTime * speed;
    worldMat = initTransform * Matrix::CreateTranslation(position)
        * Matrix::CreateRotationY(angle);

}

Vector3 TestCube::GetCenterLocation()
{
    return position;
}

void TestCube::SetInitTransform(Matrix transformMat)
{
    initTransform = transformMat;
}