#include "Graphics/Renderer/Technique/PointLightTechnique.h"
#include <Utils/StringUtils.h>

namespace SE_G {
    PointLightTechnique::PointLightTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
        eastl::string technique,
        eastl::shared_ptr<Camera> camera,
        eastl::shared_ptr<PointLightData> lightData)
        : LightTechnique(device, assignedTransform, technique, camera, lightData)
    {
        // Depth
        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthCompLess = eastl::make_shared<Bind::DepthStencilState>(device, dsDesc);

        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
        depthCompGreater = eastl::make_shared<Bind::DepthStencilState>(device, dsDesc);

        // Rasterizer
        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rastCullNone = eastl::make_shared<Bind::Rasterizer>(device, rasterDesc);

        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rastCullBack = eastl::make_shared<Bind::Rasterizer>(device, rasterDesc);

        rasterDesc.CullMode = D3D11_CULL_FRONT;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rastCullFront = eastl::make_shared<Bind::Rasterizer>(device, rasterDesc);

        m_mesh = SE_G::Mesh::CreateGeosphereMesh(device, DXSM::Vector3::One * lightData->Range, 1);
        m_vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
            device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/PointLightVShader.hlsl"));
        m_pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
            device, MakeEngineAssetPath_Wchar(L"Shaders/LightPass/PointLightPShader.hlsl"));
    }

    void PointLightTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    {
        m_lightData->Position = m_assignedTransform->m_position;
        m_lightDataBuffer->Update(context.Get(), *m_lightData);
        BindAll(context);
        DrawTechnique(context);
    }

    void PointLightTechnique::ChooseDepthStencilState(LightPosition lightPos)
    {
        if (lightPos == LightPosition::INSIDE) {
            depthStencilState = depthCompGreater;
        }
        else if (lightPos == LightPosition::FILL || lightPos == LightPosition::BEHIND_NEAR_PLANE) {
            depthStencilState = depthCompLess;
        }
        else if (lightPos == LightPosition::INTERSECT_FAR_PLANE) {
            depthStencilState = depthCompLess;
        }
        else { // I dont know why, just... whatever
            depthStencilState = depthCompGreater;
        }
    }

    void PointLightTechnique::ChooseRasterizer(LightPosition lightPos)
    {
        if (lightPos == LightPosition::INSIDE) {
            rasterizer = rastCullFront;
        }
        else if (lightPos == LightPosition::FILL || lightPos == LightPosition::BEHIND_NEAR_PLANE) {
            rasterizer = rastCullNone;
        }
        else if (lightPos == LightPosition::INTERSECT_FAR_PLANE) {
            rasterizer = rastCullBack;
        }
        else { // I dont know why, just... whatever
            rasterizer = rastCullFront;
        }
    }

    LightPosition PointLightTechnique::GetLightPositionInFrustum()
    {
        if (IsFrustumInsideOfLight())
            return LightPosition::FILL;

        Camera::FrustumPlanes planes = m_camera->GetFrustumPlanes();
        DX::XMVECTOR lightPosition = DX::XMLoadFloat3(&(m_lightData->Position));

        bool isOutside = false;
        bool intersectsFarPlane = false;
        bool behindNearPlane = false;

        // Проверка каждой плоскости фрустума
        DX::XMVECTOR planesArray[] = { planes.Near, planes.Far, planes.Left, planes.Right, planes.Top, planes.Bottom };

        for (int i = 0; i < 6; i++) {
            // Расстояние от центра сферы до плоскости
            float distance = DX::XMVectorGetX(DX::XMPlaneDotCoord(planesArray[i], lightPosition));

            // Если расстояние меньше -radius, сфера полностью вне плоскости
            if (distance <= -m_lightData->Range) {
                return LightPosition::OUTSIDE;
            }

            // Если за ближней плоскостью
            if (i == 0 && distance < 0) {
                behindNearPlane = true;
            }

            // Если пересекает дальнюю плоскость
            if (i == 1 && abs(distance) < m_lightData->Range) {
                intersectsFarPlane = true;
            }

            // Если пересекает хотя бы одну плоскость, но не полностью вне
            if (abs(distance) < m_lightData->Range) {
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

    bool PointLightTechnique::IsFrustumInsideOfLight()
    {
        Camera::FrustumCorners frustum = m_camera->GetFrustumCorners();
        for (int i = 0; i < 4; ++i) {
            DX::XMVECTOR vecToCorner = DX::XMVectorSubtract(frustum.Near[i], DXSM::Vector3(m_lightData->Position));
            float distance = DX::XMVectorGetX(DX::XMVector3Length(vecToCorner));
            if (distance > m_lightData->Range) return false;
        }
        for (int i = 0; i < 4; ++i) {
            DX::XMVECTOR vecToCorner = DX::XMVectorSubtract(frustum.Far[i], DXSM::Vector3(m_lightData->Position));
            float distance = DX::XMVectorGetX(DX::XMVector3Length(vecToCorner));
            if (distance > m_lightData->Range) return false;
        }
        return true;
    }
}