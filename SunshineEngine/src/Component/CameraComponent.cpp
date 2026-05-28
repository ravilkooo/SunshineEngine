#include <Component/CameraComponent.h>

#include <Graphics/Utils/Camera.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

CameraComponent::CameraComponent(ID3D11Device* device, TransformComponent* trComp, SE::UUID uuid)
{
    m_camera = eastl::make_shared<SE_G::Camera>(device);
    m_camera->AssignTransformComponent(trComp);
    m_camera->SetFollowUUID(uuid);
}

CameraComponent::CameraComponent(eastl::shared_ptr<SE_G::Camera> camera, TransformComponent* trComp, SE::UUID uuid)
	: m_camera(camera)
{
    m_camera->AssignTransformComponent(trComp);
    m_camera->SetFollowUUID(uuid);
}

SE_G::Camera* CameraComponent::GetCamera()
{
	return m_camera.get();
}

CameraComponent_Info::CameraComponent_Info(ID3D11Device* device, TransformComponent* trComp, SE::UUID uuid)
{
    m_assignedComponent = eastl::make_unique<CameraComponent>(device, trComp, uuid);
}

CameraComponent_Info::CameraComponent_Info(eastl::shared_ptr<SE_G::Camera> camera, TransformComponent* trComp, SE::UUID uuid)
{
    m_assignedComponent = eastl::make_unique<CameraComponent>(camera, trComp, uuid);
}

SE_G::Camera* CameraComponent_Info::GetCamera()
{
    return m_assignedComponent->m_camera.get();
}

#define CC_ADD_METHOD(k, fn) k, fn

LUA_REGISTER_COMPONENT(
    CameraComponent,
    "CameraComponent",
    /* no fields */,
    CAMERACOMPONENT_LUA_METHODS_APPLY(CC_ADD_METHOD),
    "getCameraComponent")
#undef CC_ADD_METHOD
