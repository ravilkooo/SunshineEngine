#pragma once
#include <Component/Component.h>
#include <Graphics/Utils/Camera.h>

#include <EASTL/shared_ptr.h>

class CameraComponent :
    public Component
{
public:
    CameraComponent() = default;
    CameraComponent(eastl::shared_ptr<SE_G::Camera> camera);
    ~CameraComponent() = default;

    CameraComponent(const CameraComponent&) = delete;
    CameraComponent& operator=(const CameraComponent&) = delete;

    CameraComponent(CameraComponent&&) noexcept = default;
    CameraComponent& operator=(CameraComponent&&) noexcept = default;

    const std::type_info& getType() const override {
        return typeid(CameraComponent);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::CAMERA;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    SE_G::Camera* GetCamera();

    eastl::shared_ptr<SE_G::Camera> m_camera;
};

// Macro listing methods of CameraComponent to expose in Lua bindings
#ifndef CAMERACOMPONENT_LUA_METHODS_APPLY
#define CAMERACOMPONENT_LUA_METHODS_APPLY(FM) \
    FM("getCamera", [](CameraComponent* self){ return self->GetCamera(); })
#endif
