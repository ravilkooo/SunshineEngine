#include "Graphics/Renderer/Technique/PointLightTechnique.h"
#include <Graphics/GraphicsResources/Mesh.h>

#include <ResourceManager/ResourceManagerFacade.h>

#include <Component/TransformComponent.h>

#include <Utils/StringUtils.h>
#include <Utils/MathUtils.h>

namespace SE_G {
    PointLightTechnique::PointLightTechnique(DeferredRenderer* renderer, TransformComponent* assignedTransform,
        eastl::string technique,
        eastl::shared_ptr<PointLightData> lightData)
        : LightTechnique(renderer, assignedTransform, technique, lightData)
    {
        m_depthStencilState.reset(NULL);
        m_rasterizer.reset(NULL);
        
        AssetPath meshPath = AssetPath(L"Geosphere");
        meshPath.m_params.asMesh.param1 = 1; // m_shapeData->NumSubdivisions;
        auto& rm = ResourceManagerFacade::Instance();
        ResourceHandle meshHandle = rm.LoadByPath(meshPath);
        SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
        m_mesh = eastl::shared_ptr<SE_G::Mesh>(
            meshRes,
            [](SE_G::Mesh*) {}
        );
        m_mesh->m_meshPath = meshRes->m_meshPath;

        AssetPath shaderPath = AssetPath(L"Shaders/LightPass/PointLightVShader.hlsl", AssetPath::AssetSource::Engine);
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
        //     device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/PointLightVShader.hlsl").c_str());

        // m_pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
        //     device, MakeEngineAssetPath_Wstring(L"Shaders/LightPass/PointLightPShader.hlsl").c_str());
        
        shaderPath = AssetPath(L"Shaders/LightPass/PointLightPShader.hlsl", AssetPath::AssetSource::Engine);
        shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::PIXEL_SHADER;
        ResourceHandle pshaderHandle = rm.LoadByPath(shaderPath);
        SE_G::Bind::PixelShader* pshaderRes = rm.Get<SE_G::Bind::PixelShader>(pshaderHandle);
        m_pixelShader = eastl::shared_ptr<SE_G::Bind::PixelShader>(
            pshaderRes,
            [](SE_G::Bind::PixelShader*) {}
        );

		// m_assignedTransform->m_localScaleFactor = DXSM::Vector3(m_lightData->Range, m_lightData->Range, m_lightData->Range);
    }

    void PointLightTechnique::Pass(ID3D11DeviceContext* context)
    {
        DXSM::Vector3 old_localScaleFactor = m_assignedTransform->GetLocalScaleFactor();
        DXSM::Vector3 old_localRotation = m_assignedTransform->GetLocalRotation();
        DXSM::Vector3 old_localPosition = m_assignedTransform->GetLocalPosition();

        m_assignedTransform->EnableMeshTransformMode();
        DXSM::Matrix fullTransform = m_assignedTransform->GetWorldMatrix_noLocal();
        DXSM::Vector3 scale;
        DXSM::Vector3 rotate;
        DXSM::Vector3 translation;
        DecomposeTransform(fullTransform, scale, rotate, translation);

        m_assignedTransform->SetLocalPosition(DXSM::Vector3::Zero);
        m_assignedTransform->SetLocalRotation(DXSM::Vector3::Zero);
        m_assignedTransform->SetLocalScaleFactor(DXSM::Vector3(
            1.0f / scale.x,
            1.0f / scale.y,
            1.0f / scale.z
        ));
        m_assignedTransform->BindToGraphicsPipeline(context);

        // to-do: update only when changed
        m_lightData->Position = DXSM::Vector3(fullTransform._41, fullTransform._42, fullTransform._43);
        m_lightDataVertexCBuffer->Update(context, *m_lightData);
        m_lightDataPixelCBuffer->Update(context, *m_lightData);
        BindAll(context);
        DrawTechnique(context);

        m_assignedTransform->DisableMeshTransformMode();
        m_assignedTransform->SetLocalPosition(old_localPosition);
        m_assignedTransform->SetLocalRotation(old_localRotation);
        m_assignedTransform->SetLocalScaleFactor(old_localScaleFactor);
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

    LightPosition PointLightTechnique::GetLightPositionInFrustum(Camera* camera)
    {
        if (IsFrustumInsideOfLight(camera))
            return LightPosition::FILL;

        Camera::FrustumPlanes planes = camera->GetFrustumPlanes();
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

    bool PointLightTechnique::IsFrustumInsideOfLight(Camera* camera)
    {
        Camera::FrustumCorners frustum = camera->GetFrustumCorners();
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