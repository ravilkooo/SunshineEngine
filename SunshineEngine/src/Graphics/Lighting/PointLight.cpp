#include "Graphics/Lighting/PointLight.h"


PointLight::PointLight(SE_G::PointLightData pointLightData)
{
    if (pointLightData.Att.z < 0.0001) {
        float c = eastl::max(eastl::max(pointLightData.Diffuse.x, pointLightData.Diffuse.y), pointLightData.Diffuse.z) / pointLightData.Att.y;
        pointLightData.Range = eastl::max(pointLightData.Range, (256.0f * c)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }
    else {
        float c = eastl::max(eastl::max(pointLightData.Diffuse.x, pointLightData.Diffuse.y), pointLightData.Diffuse.z) / pointLightData.Att.z;
        pointLightData.Range = eastl::max(pointLightData.Range, (16.0f * sqrtf(c) + 1.0f)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }

    m_lightData = eastl::make_shared<SE_G::PointLightData>(pointLightData);

    /*
    CreateSimpleSphereMesh(range, 6, 2,
        diffuse,
        &vertices, &verticesNum, &indices, &indicesNum);
    */
}

PointLight_Info::PointLight_Info(SE_G::PointLightData pointLightData)
{
    if (pointLightData.Att.z < 0.0001) {
        float c = eastl::max(eastl::max(pointLightData.Diffuse.x, pointLightData.Diffuse.y), pointLightData.Diffuse.z) / pointLightData.Att.y;
        pointLightData.Range = eastl::max(pointLightData.Range, (256.0f * c)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }
    else {
        float c = eastl::max(eastl::max(pointLightData.Diffuse.x, pointLightData.Diffuse.y), pointLightData.Diffuse.z) / pointLightData.Att.z;
        pointLightData.Range = eastl::max(pointLightData.Range, (16.0f * sqrtf(c) + 1.0f)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }

    m_lightData = eastl::make_shared<SE_G::PointLightData>(pointLightData);
    m_name = "PointLight";
    m_group = GameObjectGroup::Lighting;

    /*
    CreateSimpleSphereMesh(range, 6, 2,
        diffuse,
        &vertices, &verticesNum, &indices, &indicesNum);
    */
}

//void PointLight::UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
//{
//    //pointLightPBuffer->Update(context.Get(), pointLightData);
//}

/*
D3D11_DEPTH_STENCIL_DESC PointLight::ChooseDepthStencilState(LightObject::LightPosition lightPos)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
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

D3D11_RASTERIZER_DESC PointLight::GetRasterizerDesc(LightObject::LightPosition lightPos)
{
    D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
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
*/