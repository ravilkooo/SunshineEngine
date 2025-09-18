#include "SpotLight.h"
#include "Utils/wcharUtils.h"


SpotLight::SpotLight(ID3D11Device* device, Vector3 position,
    float range, Vector3 direction, float spot, Vector3 att,
    Vector4 ambient, Vector4 diffuse, Vector4 specular)
{
    this->ambient;

    if (att.z < 0.0001) {
        float c = eastl::max(eastl::max(diffuse.x, diffuse.y), diffuse.z) / att.y;
        range = eastl::max(range, (256.0f * c)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }
    else {
        float c = eastl::max(eastl::max(diffuse.x, diffuse.y), diffuse.z) / att.z;
        range = eastl::max(range, (16.0f * sqrtf(c) + 1.0f)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }

    spotLightData = {
        diffuse, specular, position, range,
        direction, spot,
        att, 0.0f
    };

    //float coneAngle = acosf(powf(256.0f, -1.0f / spot));
    //width = 2 * range * sinf(coneAngle);
    float coneAngle = acosf(powf(128.0f, -1.0f / spot));
    width = range * sinf(coneAngle);
    depth = range;
    
    CreateSimpleCubeMesh(width, width, depth, diffuse, &vertices, &verticesNum, &indices, &indicesNum);

    // GBufferPass
    {
        RenderTechnique* gBufferPass = new RenderTechnique("GBufferPass");
        gBufferPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        gBufferPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
        gBufferPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
        // AddStaticBind(texture);

        wchar_t vsFilePath[250];
        getGraphicsAssetPath(vsFilePath, 250, L"Shaders/GBufferPass/SpotLightGBufferShaderVS.hlsl");
        Bind::VertexShader* vertexShaderB = new Bind::VertexShader(device, vsFilePath);
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


        wchar_t psFilePath[250];
        getGraphicsAssetPath(psFilePath, 250, L"Shaders/GBufferPass/SpotLightGBufferShaderPS.hlsl");
        gBufferPass->AddBind(new Bind::PixelShader(device, psFilePath));


        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_NONE;
        rastDesc.FillMode = D3D11_FILL_WIREFRAME;
        gBufferPass->AddBind(new Bind::Rasterizer(device, rastDesc));
        gBufferPass->AddBind(new Bind::TransformCBuffer(device, this, 0u));

        // techniques.insert({ "GBufferPass", gBufferPass });
    }
    // LightPass
    {
        RenderTechnique* lightPass = new RenderTechnique("LightPass");
        lightPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        lightPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
        // AddStaticBind(texture);

        wchar_t vsFilePath[250];
        getGraphicsAssetPath(vsFilePath, 250, L"Shaders/LightPass/SpotLightVShader.hlsl");
        Bind::VertexShader* vertexShaderB = new Bind::VertexShader(device, vsFilePath);
        lightPass->AddBind(vertexShaderB);


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

        lightPass->AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));


        wchar_t psFilePath[250];
        getGraphicsAssetPath(psFilePath, 250, L"Shaders/LightPass/SpotLightPShader.hlsl");
        lightPass->AddBind(new Bind::PixelShader(device, psFilePath));

        /*
        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        lightPass->AddBind(new Bind::Rasterizer(device, rastDesc));
        */
        lightPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
        lightPass->AddBind(new Bind::TransformCBuffer(device, this, 0u));

        spotLightPBuffer = new Bind::PixelConstantBuffer<SpotLightPCB>(device, spotLightData, 1u);
        lightPass->AddBind(spotLightPBuffer);

        techniques.insert({ "LightPass", lightPass });
    }
}

D3D11_DEPTH_STENCIL_DESC SpotLight::GetDepthStencilDesc(LightObject::LightPosition lightPos)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    // hot fix
    {
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
        return dsDesc;
    }
    if (lightPos == LightPosition::INSIDE) {
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    }
    else if (lightPos == LightPosition::FILL || lightPos == LightPosition::BEHIND_NEAR_PLANE) {
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    }
    else if (lightPos == LightPosition::INTERSECT_FAR_PLANE) {
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    }
    else { // I dont know why, just 
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    }
    return dsDesc;
}

D3D11_RASTERIZER_DESC SpotLight::GetRasterizerDesc(LightObject::LightPosition lightPos)
{
    D3D11_RASTERIZER_DESC rasterDesc = {};

    // hot fix
    {
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        return rasterDesc;
    }

    if (lightPos == LightPosition::INSIDE) {
        rasterDesc.CullMode = D3D11_CULL_FRONT;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
    }
    else if (lightPos == LightPosition::FILL || lightPos == LightPosition::BEHIND_NEAR_PLANE) {
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
    }
    else if (lightPos == LightPosition::INTERSECT_FAR_PLANE) {
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
    }
    else { // I dont know why, just 
        rasterDesc.CullMode = D3D11_CULL_FRONT;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
    }
    return rasterDesc;
}

LightObject::LightPosition SpotLight::GetLightPositionInFrustum(Camera* camera)
{
    if (IsFrustumInsideOfLight(camera))
        return LightPosition::FILL;


    Camera::FrustumPlanes planes = camera->GetFrustumPlanes();
    XMVECTOR lightPosition = XMLoadFloat3(&(spotLightData.Position));

    bool isOutside = false;
    bool intersectsFarPlane = false;
    bool behindNearPlane = false;

    // Проверка каждой плоскости фрустума
    XMVECTOR planesArray[] = { planes.Near, planes.Far, planes.Left, planes.Right, planes.Top, planes.Bottom };

    for (int i = 0; i < 6; i++) {
        // Расстояние от центра сферы до плоскости
        float distance = XMVectorGetX(XMPlaneDotCoord(planesArray[i], lightPosition));

        // Если расстояние меньше -radius, сфера полностью вне плоскости
        if (distance <= -spotLightData.Range) {
            return LightPosition::OUTSIDE;
        }

        // Если за ближней плоскостью
        if (i == 0 && distance < 0) {
            behindNearPlane = true;
        }

        // Если пересекает дальнюю плоскость
        if (i == 1 && abs(distance) < spotLightData.Range) {
            intersectsFarPlane = true;
        }

        // Если пересекает хотя бы одну плоскость, но не полностью вне
        if (abs(distance) < spotLightData.Range) {
            isOutside = true;
        }
    }

    if (intersectsFarPlane) {
        return LightPosition::INTERSECT_FAR_PLANE;
    }

    if (behindNearPlane) {
        return LightPosition::BEHIND_NEAR_PLANE;
    }

    return isOutside ? LightPosition::INSIDE : LightPosition::OUTSIDE;

}

bool SpotLight::IsFrustumInsideOfLight(Camera* camera)
{
    Camera::FrustumCorners frustum = camera->GetFrustumCorners();
    for (int i = 0; i < 4; ++i) {
        XMVECTOR vecToCorner = XMVectorSubtract(frustum.Near[i], Vector3(spotLightData.Position));
        float distance = XMVectorGetX(XMVector3Length(vecToCorner));
        if (distance > spotLightData.Range) return false;
    }
    for (int i = 0; i < 4; ++i) {
        XMVECTOR vecToCorner = XMVectorSubtract(frustum.Far[i], Vector3(spotLightData.Position));
        float distance = XMVectorGetX(XMVector3Length(vecToCorner));
        if (distance > spotLightData.Range) return false;
    }
    return true;
}

void SpotLight::Update(float deltaTime) {
    worldMat = Matrix::CreateTranslation(0,0,depth * 0.5)
        * Matrix::CreateFromQuaternion(Quaternion::FromToRotation({0,0,1}, spotLightData.Direction))
        * Matrix::CreateTranslation(GetCenterLocation());
    return;
}

Vector3 SpotLight::GetCenterLocation() {
    return spotLightData.Position;
}

void SpotLight::UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
    spotLightPBuffer->Update(context.Get(), spotLightData);
}
