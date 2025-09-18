#include "PointLightInstanced.h"
#include "Utils/wcharUtils.h"


PointLightInstanced::PointLightInstanced()
{
}

PointLightInstanced::PointLightInstanced(ID3D11Device* device, UINT lightsCnt,
    PointLight::PointLightPCB* initLightsData, PointLightMovement* movementData,
    Vector4 ambient, float range = 64)
    : lightsCnt(lightsCnt)
{
    this->ambient;

    /*
    this->lightsData = (PointLight::PointLightPCB*) calloc(lightsCnt, sizeof(PointLight::PointLightPCB));
    memcpy(this->lightsData, initLightsData, lightsCnt * sizeof(PointLight::PointLightPCB));

    float maxRange = 1;

    for (size_t i = 0; i < lightsCnt; i++)
    {

        if (lightsData[i].Att.z < 0.0001) {
            float c = fmax(fmax(lightsData[i].Diffuse.x, lightsData[i].Diffuse.y), lightsData[i].Diffuse.z)
                / lightsData[i].Att.y;
            lightsData[i].Range = max(lightsData[i].Range, (256.0f * c)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
            maxRange = fminf(fmaxf(maxRange, lightsData[i].Range), 256);
        }
        else {
            float c = fmax(fmax(lightsData[i].Diffuse.x, lightsData[i].Diffuse.y), lightsData[i].Diffuse.z) / lightsData[i].Att.z;
            lightsData[i].Range = max(lightsData[i].Range, (16.0f * sqrtf(c) + 1.0f)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
            maxRange = fminf(fmaxf(maxRange, lightsData[i].Range), 256);
        }
    }
    */

    CreateSimpleSphereMesh(range, 6, 2,
        Vector4{0,0,1,1},
        &vertices, &verticesNum, &indices, &indicesNum);

    // LightPass
    {
        RenderTechnique* lightPass = new RenderTechnique("LightPass");
        lightPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        lightPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
        // AddStaticBind(texture);

        wchar_t vsFilePath[250];
        getGraphicsAssetPath(vsFilePath, 250, L"Shaders/LightPass/PointLightInstancedVShader.hlsl");
        Bind::VertexShader* vertexShaderB = new Bind::VertexShader(device, vsFilePath);
        lightPass->AddBind(vertexShaderB);


        numInputElements = 7;
        IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));

        IALayoutInputElements[0] =
            D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

        // PointLight::PointLightPCB
        IALayoutInputElements[1] =
            D3D11_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
        IALayoutInputElements[2] =
            D3D11_INPUT_ELEMENT_DESC{ "COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
        IALayoutInputElements[3] =
            D3D11_INPUT_ELEMENT_DESC{ "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
        IALayoutInputElements[4] =
            D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32_FLOAT, 1, 44, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
        IALayoutInputElements[5] =
            D3D11_INPUT_ELEMENT_DESC{ "POSITION", 2, DXGI_FORMAT_R32G32B32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
        IALayoutInputElements[6] =
            D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 1, DXGI_FORMAT_R32_FLOAT, 1, 60, D3D11_INPUT_PER_INSTANCE_DATA, 1 };

        lightPass->AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));


        wchar_t psFilePath[250];
        getGraphicsAssetPath(psFilePath, 250, L"Shaders/LightPass/PointLightInstancedPShader.hlsl");
        lightPass->AddBind(new Bind::PixelShader(device, psFilePath));

        lightPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
        lightPass->AddBind(new Bind::TransformCBuffer(device, this, 0u));
        
        instanceBuffer = new Bind::VertexBuffer(device, initLightsData, this->lightsCnt, sizeof(PointLight::PointLightPCB),
            Bind::VertexBuffer::VBType::INSTANCED_CPU, 1u);
        lightPass->AddBind(instanceBuffer);
        //lightPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));


        techniques.insert({ "LightPass", lightPass });
    }


    // LightPass
    {
        RenderTechnique* gBufferPass = new RenderTechnique("GBufferPass");
        gBufferPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        gBufferPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
        // AddStaticBind(texture);
        Bind::VertexShader* vertexShaderB = new Bind::VertexShader(device, L"Shaders/GBufferPass/PointLightInstancedGBufferVS.hlsl");
        gBufferPass->AddBind(vertexShaderB);


        numInputElements = 10;
        IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));

        IALayoutInputElements[0] =
            D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        IALayoutInputElements[1] =
            D3D11_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        IALayoutInputElements[2] =
            D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        IALayoutInputElements[3] =
            D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 };


        // PointLight::PointLightPCB
        IALayoutInputElements[4] =
            D3D11_INPUT_ELEMENT_DESC{ "COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
        IALayoutInputElements[5] =
            D3D11_INPUT_ELEMENT_DESC{ "COLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
        IALayoutInputElements[6] =
            D3D11_INPUT_ELEMENT_DESC{ "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
        IALayoutInputElements[7] =
            D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 1, DXGI_FORMAT_R32_FLOAT, 1, 44, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
        IALayoutInputElements[8] =
            D3D11_INPUT_ELEMENT_DESC{ "POSITION", 2, DXGI_FORMAT_R32G32B32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
        IALayoutInputElements[9] =
            D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 2, DXGI_FORMAT_R32_FLOAT, 1, 60, D3D11_INPUT_PER_INSTANCE_DATA, 1 };

        gBufferPass->AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));

        gBufferPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
        gBufferPass->AddBind(new Bind::TransformCBuffer(device, this, 0u));

        gBufferPass->AddBind(instanceBuffer);

        gBufferPass->AddBind(new Bind::PixelShader(device, L"Shaders/GBufferPass/PointLightInstancedGBufferPS.hlsl"));

        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        gBufferPass->AddBind(new Bind::Rasterizer(device, rastDesc));


        techniques.insert({ "GBufferPass", gBufferPass });
    }

    // PointLight Data

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = this->lightsCnt * sizeof(PointLight::PointLightPCB);
    bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bd.StructureByteStride = sizeof(PointLight::PointLightPCB);
    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = initLightsData;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;
    device->CreateBuffer(&bd, &InitData, &pPointLightData);

    D3D11_UNORDERED_ACCESS_VIEW_DESC pointLightDataUAVDesc;
    pointLightDataUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
    pointLightDataUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    pointLightDataUAVDesc.Buffer.FirstElement = 0;
    pointLightDataUAVDesc.Buffer.NumElements = this->lightsCnt;
    pointLightDataUAVDesc.Buffer.Flags = 0;
    device->CreateUnorderedAccessView(pPointLightData.Get(), &pointLightDataUAVDesc, &pPointLightDataUAV);

    // Movement

    D3D11_BUFFER_DESC movementBufferDesc;
    movementBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    movementBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    movementBufferDesc.CPUAccessFlags = 0;
    movementBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    movementBufferDesc.ByteWidth = sizeof(PointLightMovement) * lightsCnt;
    movementBufferDesc.StructureByteStride = sizeof(PointLightMovement);
    //D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = movementData;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;
    device->CreateBuffer(&movementBufferDesc, &InitData, &pMovementBuffer);

    D3D11_UNORDERED_ACCESS_VIEW_DESC movementUAVDesc;
    movementUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
    movementUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    movementUAVDesc.Buffer.FirstElement = 0;
    movementUAVDesc.Buffer.NumElements = lightsCnt;
    movementUAVDesc.Buffer.Flags = 0;
    device->CreateUnorderedAccessView(pMovementBuffer.Get(), &movementUAVDesc, &pMovementBufferUAV);

    SetUpdateComputeShader(device, L"Shaders/PointLightCS/SimulatePointLightCShader.hlsl");

    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof(PointLightInstancedCB) + (16 - (sizeof(PointLightInstancedCB) % 16));  // aligned size
    cbd.StructureByteStride = 0u;

    device->CreateBuffer(&cbd, nullptr, &pPointLightConstBufferData);
}

D3D11_DEPTH_STENCIL_DESC PointLightInstanced::GetDepthStencilDesc(LightObject::LightPosition lightPos)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    return dsDesc;
}

D3D11_RASTERIZER_DESC PointLightInstanced::GetRasterizerDesc(LightObject::LightPosition lightPos)
{
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.CullMode = D3D11_CULL_FRONT;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    return rasterDesc;
}

LightObject::LightPosition PointLightInstanced::GetLightPositionInFrustum(Camera* camera)
{
    return LightPosition::FILL;
}

bool PointLightInstanced::IsFrustumInsideOfLight(Camera* camera)
{
    return true;
}

void PointLightInstanced::DrawTechnique(std::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const noexcept
{
    context->DrawIndexedInstanced(indicesNum, lightsCnt, 0, 0, 0);
}

void PointLightInstanced::Update(float deltaTime)
{
    pliConstBufferData.dt = deltaTime;
    pliConstBufferData.pointLightCount = lightsCnt;
    pliConstBufferData.rngSeed = (float)std::rand();
}

Vector3 PointLightInstanced::GetCenterLocation()
{
    return Vector3::Zero;
}

inline int align(int value, int alignment) { return (value + (alignment - 1)) & ~(alignment - 1); };

void PointLightInstanced::UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(pPointLightConstBufferData.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
    memcpy(mappedResource.pData, &pliConstBufferData, sizeof(pliConstBufferData) + (16 - (sizeof(pliConstBufferData) % 16))); // aligned size
    context->Unmap(pPointLightConstBufferData.Get(), 0);
    context->CSSetConstantBuffers(0u, 1u, pPointLightConstBufferData.GetAddressOf());

    UINT initialCount[] = { (UINT)-1 };
    context->CSSetUnorderedAccessViews(0u, 1u, pPointLightDataUAV.GetAddressOf(), initialCount);
    context->CSSetUnorderedAccessViews(1u, 1u, pMovementBufferUAV.GetAddressOf(), initialCount);
    context->CSSetShader(pUpdatePointLightDataComputeShader.Get(), nullptr, 0);

    context->Dispatch(align(this->lightsCnt, 256) / 256, 1, 1);

    context->CSSetShader(nullptr, nullptr, 0);
    ID3D11UnorderedAccessView* uavsNull[] = { nullptr, nullptr };
    context->CSSetUnorderedAccessViews(0u, 2u, uavsNull, nullptr);

    instanceBuffer->Update(context.Get(), pPointLightData.Get());
}

void PointLightInstanced::SetUpdateComputeShader(ID3D11Device* device, LPCWSTR filePath)
{
    Microsoft::WRL::ComPtr<ID3DBlob> cs_blob;

    D3DCompileFromFile(filePath,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", "cs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
        &cs_blob, nullptr);
    device->CreateComputeShader(
        cs_blob->GetBufferPointer(),
        cs_blob->GetBufferSize(),
        nullptr,
        pUpdatePointLightDataComputeShader.GetAddressOf()
    );
}