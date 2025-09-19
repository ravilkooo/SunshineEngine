#include "LightObjects/DirectionalLight.h"
#include "Utils/wcharUtils.h"

DirectionalLight::DirectionalLight(ID3D11Device* device, Vector3 position,
    Vector3 direction, Vector4 ambient,
    Vector4 diffuse, Vector4 specular)
{
    direction.Normalize();
    this->ambient = ambient;
    directionalLightData = {
        diffuse, specular, position, 0.0f,
        direction, 0.0f
    };

    indices = (int*)calloc(4, sizeof(int));
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;

    indicesNum = 4;

    // LightPass
    {
        RenderTechnique* lightPass = new RenderTechnique("LightPass");
        lightPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP));
        lightPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
        // AddStaticBind(texture);

        wchar_t vsFilePath[250];
        getGraphicsAssetPath(vsFilePath, 250, L"Shaders/LightPass/DirectionalLightVShader.hlsl");
        lightPass->AddBind(new Bind::VertexShader(device, vsFilePath));

        wchar_t psFilePath[250];
        getGraphicsAssetPath(psFilePath, 250, L"Shaders/LightPass/DirectionalLightPShader.hlsl");
        lightPass->AddBind(new Bind::PixelShader(device, psFilePath));

        /*
        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        lightPass->AddBind(new Bind::Rasterizer(device, rastDesc));
        */
        directionalLightPBuffer = new Bind::PixelConstantBuffer<DirectionalLightPCB>(device, directionalLightData, 1u);
        lightPass->AddBind(directionalLightPBuffer);
        //std::cout << sizeof(directionalLightData) << "\n";
        //std::cout << sizeof(directionalLightData) + (16 - (sizeof(directionalLightData) % 16)) % 16 << "\n";

        techniques.insert({ "LightPass", lightPass });
    }
}

D3D11_DEPTH_STENCIL_DESC DirectionalLight::GetDepthStencilDesc(LightObject::LightPosition lightPos)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    return dsDesc;
}

D3D11_RASTERIZER_DESC DirectionalLight::GetRasterizerDesc(LightObject::LightPosition lightPos)
{
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    return rasterDesc;
}

LightObject::LightPosition DirectionalLight::GetLightPositionInFrustum(Camera* camera)
{
    return LightPosition::FILL;
}

bool DirectionalLight::IsFrustumInsideOfLight(Camera* camera)
{
    return true;
}

void DirectionalLight::Update(float deltaTime)
{
}

Vector3 DirectionalLight::GetCenterLocation()
{
    return directionalLightData.Position;
}

void DirectionalLight::UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
    directionalLightPBuffer->Update(context.Get(), directionalLightData);
}

