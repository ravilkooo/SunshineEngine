#pragma once
#include "SunshineEngineAPI.h"
#include "Component.h"
#include <SimpleMath.h>
#include <d3d11.h>
#include <Graphics/Bindable/TransformCBuffer.h>
#include <EASTL/unique_ptr.h>

namespace DXSM = DirectX::SimpleMath;

class SUNSHINE_ENGINE_API TransformComponent :
    public Component
{
    friend class TransformComponent_Info;
public:
    TransformComponent() {};    
    ~TransformComponent();

    TransformComponent(ID3D11Device* device);

    void SetupBuffer(ID3D11Device* device);

    eastl::unique_ptr<SE_G::Bind::TransformCBuffer> transformBuffer;

    void BindToGraphicsPipeline(ID3D11DeviceContext* context);

    /*
    // ================
    // To make SUNSHINE_ENGINE_API work

    TransformComponent(TransformComponent&&) noexcept = default;
    TransformComponent& operator=(TransformComponent&&) noexcept = default;

    TransformComponent(const TransformComponent&) = delete;
    TransformComponent& operator=(const TransformComponent&) = delete;
    // ================
    */

    // Transforms - usual as in every engine: translate+rotate+scale
    // ...Transfrom()
    // +
    // Additional local transforms only for object: translate+rotate+scale (for local corrections etc.)
    // ...LocalTransfrom()

    // Transforms
    DXSM::Matrix GetTransalationMatrix() const;
    DXSM::Matrix GetRotationMatrix() const;
    DXSM::Matrix GetScaleMatrix() const;

    // Additional Transforms
    DXSM::Matrix GetLocalTransalationMatrix() const;
    DXSM::Matrix GetLocalRotationMatrix() const;
    DXSM::Matrix GetLocalScaleMatrix() const;

    DXSM::Matrix GetLocalTransformMatrix() const;

    // World Transform
    DXSM::Matrix GetWorldMatrix() const; // include LocalTransfrom

    // Transform
    DXSM::Vector3 m_position = { 0, 0, 0 };
    DXSM::Vector3 m_rotation = { 0, 0, 0 }; // Pitch (x-axis), Yaw (y-axis), Roll (z-axis)
    DXSM::Vector3 m_scaleFactor = { 1, 1, 1 };

    // Local Transform
    DXSM::Vector3 m_localPosition = { 0, 0, 0 };
    DXSM::Vector3 m_localRotation = { 0, 0, 0 }; // Pitch (x-axis), Yaw (y-axis), Roll (z-axis)
    DXSM::Vector3 m_localScaleFactor = { 1, 1, 1 };

    DXSM::Matrix localTransfrom = DXSM::Matrix::Identity;

    const std::type_info& getType() const override {
        return typeid(TransformComponent);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::TRANSFORM;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    // Serialization
    void FromJson(const json& j) override;
    //void FromJson(const json& j, ID3D11Device* device);
};

class TransformComponent_Info : public Component_Info
{
public:
    TransformComponent_Info() {};
    TransformComponent_Info(ID3D11Device* device);
    ~TransformComponent_Info();

    static const SE::ComponentType s_componentType = SE::ComponentType::TRANSFORM;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    const std::type_info& getType() const override {
        return typeid(TransformComponent_Info);
    }

    bool IsAssigned() override { return true; }

    eastl::unique_ptr<TransformComponent> m_assignedComponent;

    // Serialization
    json ToJson() const override;
    //void FromJson(const json& j) override;
    void FromJson(const json& j, ID3D11Device* device);
};

// Macro listing fields of TransformComponent to expose in Lua bindings
#ifndef TRANSFORMCOMPONENT_LUA_FIELDS_APPLY
#define TRANSFORMCOMPONENT_LUA_FIELDS_APPLY(F) \
    F(m_position) ,                         \
    F(m_rotation) ,                         \
    F(m_scaleFactor) ,                      \
    F(m_localPosition) ,                    \
    F(m_localRotation) ,                    \
    F(m_localScaleFactor)
#endif