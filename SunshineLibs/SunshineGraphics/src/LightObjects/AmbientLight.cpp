#include "LightObjects/AmbientLight.h"
#include "Utils/wcharUtils.h"

AmbientLight::AmbientLight(ID3D11Device* device, Vector4 ambient)
{
    this->ambient = ambient;
    ambientLightData = {
        ambient
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
        getGraphicsAssetPath(vsFilePath, 250, L"Shaders/LightPass/AmbientLightVShader.hlsl");
        lightPass->AddBind(new Bind::VertexShader(device, vsFilePath));

        wchar_t psFilePath[250];
        getGraphicsAssetPath(psFilePath, 250, L"Shaders/LightPass/AmbientLightPShader.hlsl");
        lightPass->AddBind(new Bind::PixelShader(device, psFilePath));

        /*
        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        lightPass->AddBind(new Bind::Rasterizer(device, rastDesc));
        */
        ambientLightPBuffer = new Bind::PixelConstantBuffer<AmbientLightPCB>(device, ambientLightData, 1u);
        lightPass->AddBind(ambientLightPBuffer);
        //std::cout << sizeof(AmbientLightData) << "\n";
        //std::cout << sizeof(AmbientLightData) + (16 - (sizeof(AmbientLightData) % 16)) % 16 << "\n";

        techniques.insert({ "LightPass", lightPass });
    }
}

D3D11_DEPTH_STENCIL_DESC AmbientLight::GetDepthStencilDesc(LightObject::LightPosition lightPos)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    return dsDesc;
}

D3D11_RASTERIZER_DESC AmbientLight::GetRasterizerDesc(LightObject::LightPosition lightPos)
{
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    return rasterDesc;
}

LightObject::LightPosition AmbientLight::GetLightPositionInFrustum(Camera* camera)
{
    return LightPosition::FILL;
}

bool AmbientLight::IsFrustumInsideOfLight(Camera* camera)
{
    return true;
}

void AmbientLight::Update(float deltaTime)
{
}

Vector3 AmbientLight::GetCenterLocation()
{
    return Vector3::Zero;
}

void AmbientLight::UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
    ambientLightPBuffer->Update(context.Get(), ambientLightData);
}

