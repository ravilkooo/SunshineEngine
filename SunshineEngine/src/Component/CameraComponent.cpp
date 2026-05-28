#include <Component/CameraComponent.h>

#include <Graphics/Utils/Camera.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

CameraComponent::CameraComponent(eastl::shared_ptr<SE_G::Camera> camera)
	: m_camera(camera)
{

}

SE_G::Camera* CameraComponent::GetCamera()
{
	return m_camera.get();
}

CameraComponent_Info::CameraComponent_Info(eastl::shared_ptr<SE_G::Camera> camera)
{
    m_assignedComponent = eastl::make_unique<CameraComponent>(camera);
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
