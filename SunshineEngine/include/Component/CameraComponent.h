#pragma once
#include <Component/Component.h>

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <Utils/UUID.h>

class ID3D11Device;
class TransformComponent;

namespace SE_G
{
    class Camera;
}

class CameraComponent :
    public Component
{
public:
    CameraComponent() = default;
    CameraComponent(ID3D11Device* device, TransformComponent* trComp, SE::UUID uuid);
    CameraComponent(eastl::shared_ptr<SE_G::Camera> camera, TransformComponent* trComp, SE::UUID uuid);
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


class CameraComponent_Info :
    public Component_Info
{
public:
    CameraComponent_Info() = default;
    CameraComponent_Info(ID3D11Device* device, TransformComponent* trComp, SE::UUID uuid);
    CameraComponent_Info(eastl::shared_ptr<SE_G::Camera> camera, TransformComponent* trComp, SE::UUID uuid);
    ~CameraComponent_Info() = default;

    CameraComponent_Info(const CameraComponent_Info&) = delete;
    CameraComponent_Info& operator=(const CameraComponent_Info&) = delete;

    CameraComponent_Info(CameraComponent_Info&&) noexcept = default;
    CameraComponent_Info& operator=(CameraComponent_Info&&) noexcept = default;

    const std::type_info& getType() const override {
        return typeid(CameraComponent_Info);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::CAMERA;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    SE_G::Camera* GetCamera();

    bool IsAssigned() override { return true; }
    eastl::unique_ptr<CameraComponent> m_assignedComponent;
};

// Macro listing methods of CameraComponent to expose in Lua bindings
#ifndef CAMERACOMPONENT_LUA_METHODS_APPLY
#define CAMERACOMPONENT_LUA_METHODS_APPLY(FM) \
    FM("getCamera", [](CameraComponent* self){ return self->GetCamera(); })
#endif
