#include <Component/CameraComponent.h>

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

#define CC_ADD_METHOD(k, fn) k, fn

LUA_REGISTER_COMPONENT(
    CameraComponent,
    "CameraComponent",
    /* no fields */,
    CAMERACOMPONENT_LUA_METHODS_APPLY(CC_ADD_METHOD),
    "getCameraComponent")
#undef CC_ADD_METHOD
