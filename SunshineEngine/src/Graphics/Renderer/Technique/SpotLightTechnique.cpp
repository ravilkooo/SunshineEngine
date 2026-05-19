#include "Graphics/Renderer/Technique/SpotLightTechnique.h"
#include <Graphics/GraphicsResources/Mesh.h>

#include <ResourceManager/ResourceManagerFacade.h>

#include <Component/TransformComponent.h>

#include <Utils/StringUtils.h>

namespace SE_G {

    SpotLightTechnique::SpotLightTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
        eastl::string technique,
        eastl::shared_ptr<Camera> camera,
        eastl::shared_ptr<SpotLightData> lightData)
        : LightTechnique(device, assignedTransform, technique, camera, lightData)
    {
        m_depthStencilState.reset(NULL);
        m_rasterizer.reset(NULL);

        float coneAngle = acosf(powf(128.0f, -1.0f / lightData->Spot));
        float width = lightData->Range * sinf(coneAngle);
        float depth = lightData->Range;

        AssetPath meshPath = AssetPath(L"Box");
        auto& rm = ResourceManagerFacade::Instance();
        ResourceHandle meshHandle = rm.LoadByPath(meshPath);
        SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
        m_mesh = eastl::shared_ptr<SE_G::Mesh>(
            meshRes,
            [](SE_G::Mesh*) {}
        );
        m_mesh->m_meshPath = meshRes->m_meshPath;

        AssetPath shaderPath = AssetPath(L"Shaders/LightPass/SpotLightVShader.hlsl", AssetPath::AssetSource::Engine);
        shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::VERTEX_SHADER;
        SE_G::Bind::VertexShader::FillStandartInputLayout(shaderPath.m_params.asShader.numInputElements,
            shaderPath.m_params.asShader.IALayoutInputElements);
        ResourceHandle vshaderHandle = rm.LoadByPath(shaderPath);
        SE_G::Bind::VertexShader* vshaderRes = rm.Get<SE_G::Bind::VertexShader>(vshaderHandle);
        m_vertexShader = eastl::shared_ptr<SE_G::Bind::VertexShader>(
            vshaderRes,
            [](SE_G::Bind::VertexShader*) {}
        );
        delete[] shaderPath.m_params.asShader.IALayoutInputElements;
        // m_vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
        //     device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/SpotLightVShader.hlsl").c_str());

        // m_pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
        //     device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/SpotLightPShader.hlsl").c_str());

        shaderPath = AssetPath(L"Shaders/LightPass/SpotLightPShader.hlsl", AssetPath::AssetSource::Engine);
        shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::PIXEL_SHADER;
        ResourceHandle pshaderHandle = rm.LoadByPath(shaderPath);
        SE_G::Bind::PixelShader* pshaderRes = rm.Get<SE_G::Bind::PixelShader>(pshaderHandle);
        m_pixelShader = eastl::shared_ptr<SE_G::Bind::PixelShader>(
            pshaderRes,
            [](SE_G::Bind::PixelShader*) {}
        );

		// m_assignedTransform->m_localScaleFactor = DXSM::Vector3(width, depth, width);
    }

    void SpotLightTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    {
        // to-do: update only when changed
        auto wMat = m_assignedTransform->GetWorldMatrix();
        m_lightData->Position = DXSM::Vector3(wMat._41, wMat._42, wMat._43);

        DXSM::Matrix rot = m_assignedTransform->GetRotationMatrix();
        DXSM::Vector3 dir = DXSM::Vector3::Transform(DXSM::Vector3::Down, rot);
        float h = asinf(dir.y);
        float eps = 0.001f;
        float az = (h > (1.0f - eps)) ? atan2f(dir.z, dir.x) : 0.0f;
        m_lightData->Direction = { az, h };

        m_lightDataVertexCBuffer->Update(context.Get(), *m_lightData);
        m_lightDataPixelCBuffer->Update(context.Get(), *m_lightData);
        BindAll(context);
        DrawTechnique(context);
    }

    void SpotLightTechnique::ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos)
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

    void SpotLightTechnique::ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos)
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

    LightPosition SpotLightTechnique::GetLightPositionInFrustum()
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

    bool SpotLightTechnique::IsFrustumInsideOfLight()
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