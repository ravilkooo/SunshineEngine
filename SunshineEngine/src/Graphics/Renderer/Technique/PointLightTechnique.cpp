#include "Graphics/Renderer/Technique/PointLightTechnique.h"
#include <Graphics/GraphicsResources/Mesh.h>
#include <Utils/StringUtils.h>

namespace SE_G {
    PointLightTechnique::PointLightTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
        eastl::string technique,
        eastl::shared_ptr<Camera> camera,
        eastl::shared_ptr<PointLightData> lightData)
        : LightTechnique(device, assignedTransform, technique, camera, lightData)
    {
        m_depthStencilState.reset(NULL);
        m_rasterizer.reset(NULL);
        m_mesh = SE_G::Mesh::CreateGeosphereMesh(device, DXSM::Vector3::One * lightData->Range, 1);
        m_vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
            device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/PointLightVShader.hlsl").c_str());
        m_pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
            device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/PointLightPShader.hlsl").c_str());
    }

    void PointLightTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    {
        // to-do: update only when changed
        m_lightData->Position = m_assignedTransform->m_position;
        m_lightDataBuffer->Update(context.Get(), *m_lightData);
        BindAll(context);
        DrawTechnique(context);
    }

    void PointLightTechnique::ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos)
    {
        if (lightPos == LightPosition::INSIDE) {
            LightStaticData::depthCompGreater->Bind(context);
        }
        else if (lightPos == LightPosition::FILL || lightPos == LightPosition::BEHIND_NEAR_PLANE) {
            LightStaticData::depthCompGreater->Bind(context);
        }
        else if (lightPos == LightPosition::INTERSECT_FAR_PLANE) {
            LightStaticData::depthCompLess->Bind(context);
        }
        else { // I dont know why, just... whatever
            LightStaticData::depthCompGreater->Bind(context);
        }
    }

    void PointLightTechnique::ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos)
    {
        if (lightPos == LightPosition::INSIDE) {
            LightStaticData::rastCullFront->Bind(context);
        }
        else if (lightPos == LightPosition::FILL || lightPos == LightPosition::BEHIND_NEAR_PLANE) {
            LightStaticData::rastCullNone->Bind(context);
        }
        else if (lightPos == LightPosition::INTERSECT_FAR_PLANE) {
            LightStaticData::rastCullBack->Bind(context);
        }
        else { // I dont know why, just... whatever
            LightStaticData::rastCullNone->Bind(context);
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