#include <GameObject/GameObjectFactory.h>
#include <GameObject/GameObject.h>
#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>
#include <GameObject/Lighting/AmbientLight.h>
#include <GameObject/Lighting/PointLight.h>
#include <GameObject/Lighting/DirectionalLight.h>
#include <GameObject/Lighting/SkyBox.h>
#include <GameObject/Shapes/ShapeData.h>

#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/Technique/GPassTechnique.h>
#include <Graphics/Renderer/Technique/LightTechnique.h>
#include <Graphics/Renderer/Technique/AmbientLightTechnique.h>
#include <Graphics/Renderer/Technique/DirectionalLightTechnique.h>
#include <Graphics/Renderer/Technique/PointLightTechnique.h>
#include <Graphics/Renderer/Technique/SkyBoxTechnique.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>

#include <Serialization/ShapeSerialization.h>


eastl::unique_ptr<GameObject> GameObjectFactory::CreateBoxObject(
    SE_G::DeferredRenderer* renderSystem,
    float width, float height, float length)
{
    auto device = renderSystem->GetDevice();

    auto obj = eastl::make_unique<GameObject>();
    auto tr = obj->AddComponent<TransformComponent>();
    auto rc = obj->AddComponent<RenderComponent>(renderSystem);

    auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
        renderSystem, tr.get(), "GPass", obj->m_UUID);

    gBufferTech->m_mesh = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(
        device, DXSM::Vector3(width, height, length));
    gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultTexture.dds"));

    rc->AddTechnique(eastl::move(gBufferTech));

    return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateBoxObject(
    SE_G::DeferredRenderer* renderSystem,
    const json& j)
{
    eastl::unique_ptr<GameObject> obj = eastl::make_unique<GameObject>();

    obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
    obj->m_name = "Box";

    auto device = renderSystem->GetDevice();

    // TransformComponent
    auto tc = obj->AddComponent<TransformComponent>(renderSystem->GetDevice());
    if (j["components"].contains("Transform")) {
        tc->FromJson(j["components"]["Transform"]);
    }

    auto shapeData = eastl::make_shared<BoxShapeData>(j["m_shapeData"].get<BoxShapeData>());

    // RenderComponent and technique
    auto rc = obj->AddComponent<RenderComponent>(renderSystem);

    auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
        renderSystem, tc.get(), "GPass", obj->m_UUID);
    gBufferTech->m_mesh = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(
        device, shapeData->Size);
    gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultTexture.dds"));
    rc->AddTechnique(eastl::move(gBufferTech));

    return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateSphereObject(
    SE_G::DeferredRenderer* renderSystem, float radius)
{
    auto device = renderSystem->GetDevice();

    auto obj = eastl::make_unique<GameObject>();
    auto tr = obj->AddComponent<TransformComponent>(device);
    auto rc = obj->AddComponent<RenderComponent>(renderSystem);

    auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
        renderSystem, tr.get(), "GPass", obj->m_UUID);
    gBufferTech->m_mesh = SE_G::Mesh::CreateSphereMesh(
        device, DXSM::Vector3::One * radius);
    gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"));

    rc->AddTechnique(eastl::move(gBufferTech));

    return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateSphereObject(
    SE_G::DeferredRenderer* renderSystem,
    const json& j)
{
    eastl::unique_ptr<GameObject> obj = eastl::make_unique<GameObject>();

    obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
    obj->m_name = "Sphere";

    auto device = renderSystem->GetDevice();

    // TransformComponent
    auto tc = obj->AddComponent<TransformComponent>(renderSystem->GetDevice());
    if (j["components"].contains("Transform")) {
        tc->FromJson(j["components"]["Transform"]);
    }

    auto shapeData = eastl::make_shared<SphereShapeData>(j["m_shapeData"].get<SphereShapeData>());

    // RenderComponent and technique
    auto rc = obj->AddComponent<RenderComponent>(renderSystem);

    auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
        renderSystem, tc.get(), "GPass", obj->m_UUID);
    gBufferTech->m_mesh = SE_G::Mesh::CreateSphereMesh(
        device, shapeData->Size, shapeData->SliceCount, shapeData->StackCount);
    gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"));
    rc->AddTechnique(eastl::move(gBufferTech));

    return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateGeosphereObject(
    SE_G::DeferredRenderer* renderSystem, float radius)
{
    auto device = renderSystem->GetDevice();

    auto obj = eastl::make_unique<GameObject>();
    auto tr = obj->AddComponent<TransformComponent>(device);
    auto rc = obj->AddComponent<RenderComponent>(renderSystem);

    auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
        renderSystem, tr.get(), "GPass", obj->m_UUID);
    gBufferTech->m_mesh = SE_G::Mesh::CreateGeosphereMesh(
        device, DXSM::Vector3::One * radius, 2u);
    gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"));

    rc->AddTechnique(eastl::move(gBufferTech));

    return obj;
}

eastl::unique_ptr<GameObject> GameObjectFactory::CreateGeosphereObject(
    SE_G::DeferredRenderer* renderSystem,
    const json& j)
{
    eastl::unique_ptr<GameObject> obj = eastl::make_unique<GameObject>();

    obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
    obj->m_name = "Sphere";

    auto device = renderSystem->GetDevice();

    // TransformComponent
    auto tc = obj->AddComponent<TransformComponent>(renderSystem->GetDevice());
    if (j["components"].contains("Transform")) {
        tc->FromJson(j["components"]["Transform"]);
    }

    auto shapeData = eastl::make_shared<GeosphereShapeData>(
        j["m_shapeData"].get<GeosphereShapeData>());

    // RenderComponent and technique
    auto rc = obj->AddComponent<RenderComponent>(renderSystem);

    auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
        renderSystem, tc.get(), "GPass", obj->m_UUID);
    gBufferTech->m_mesh = SE_G::Mesh::CreateGeosphereMesh(
        device, shapeData->Size, shapeData->NumSubdivisions);
    gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"));
    rc->AddTechnique(eastl::move(gBufferTech));

    return obj;
}

eastl::unique_ptr<SkyBox> GameObjectFactory::CreateSkyBox(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    eastl::wstring texturePath,
    SE_G::SkyBoxData initData)
{
    auto obj = eastl::make_unique<SkyBox>(renderSystem, camera, texturePath, initData);
    return obj;
}

eastl::unique_ptr<SkyBox> GameObjectFactory::CreateSkyBox(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{
    auto obj = eastl::make_unique<SkyBox>(renderSystem, camera, j);
    return obj;
}

eastl::unique_ptr<AmbientLight> GameObjectFactory::CreateAmbientLightObject(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    SE_G::AmbientLightData initData)
{
    auto obj = eastl::make_unique<AmbientLight>(renderSystem, camera, initData);
    return obj;
}

eastl::unique_ptr<AmbientLight> GameObjectFactory::CreateAmbientLightObject(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{
    auto obj = eastl::make_unique<AmbientLight>(renderSystem, camera, j);
    return obj;
}

eastl::unique_ptr<DirectionalLight> GameObjectFactory::CreateDirectionalLightObject(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    SE_G::DirectionalLightData initData)
{
    auto obj = eastl::make_unique<DirectionalLight>(renderSystem, camera, initData);
    return obj;
}

eastl::unique_ptr<DirectionalLight> GameObjectFactory::CreateDirectionalLightObject(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{
    auto obj = eastl::make_unique<DirectionalLight>(renderSystem, camera, j);
    return obj;
}

eastl::unique_ptr<PointLight> GameObjectFactory::CreatePointLightObject(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    SE_G::PointLightData initData)
{
    auto obj = eastl::make_unique<PointLight>(renderSystem, camera, initData);
    return obj;
}

eastl::unique_ptr<PointLight> GameObjectFactory::CreatePointLightObject(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{
    auto obj = eastl::make_unique<PointLight>(renderSystem, camera, j);
    return obj;
}
