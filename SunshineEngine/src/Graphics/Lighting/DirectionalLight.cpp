#include "Graphics/Lighting/DirectionalLight.h"

DirectionalLight::DirectionalLight(
    DirectionalLightData directionalLightData)
{
    directionalLightData.Direction.Normalize();
    
    this->directionalLightData = eastl::make_shared<DirectionalLightData>(directionalLightData);
}

//void DirectionalLight::UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
//{
//    //directionalLightPBuffer->Update(context.Get(), directionalLightData);
//}

/*
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
        
        lightPass->mesh = std::make_shared<Mesh>();
        lightPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));

        wchar_t vsFilePath[250];
        getGraphicsAssetPath(vsFilePath, 250, L"Shaders/LightPass/DirectionalLightVShader.hlsl");
        //lightPass->vertexShader = std::make_shared<Bind::VertexShader>(device, vsFilePath);
        lightPass->AddBind(new Bind::VertexShader(device, vsFilePath));

        wchar_t psFilePath[250];
        getGraphicsAssetPath(psFilePath, 250, L"Shaders/LightPass/DirectionalLightPShader.hlsl");
        //lightPass->pixelShader = std::make_shared<Bind::PixelShader>(device, psFilePath);
        lightPass->AddBind(new Bind::PixelShader(device, psFilePath));

        directionalLightPBuffer = new Bind::PixelConstantBuffer<DirectionalLightPCB>(device, directionalLightData, 1u);
        lightPass->AddBind(directionalLightPBuffer);



        techniñs.insert({ "LightPass", lightPass });
    }
}

D3D11_DEPTH_STENCIL_DESC DirectionalLight::ChooseDepthStencilState(LightObject::LightPosition lightPos)
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
*/
