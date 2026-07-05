#pragma once
#include "SunshineEngineAPI.h"
#include "Component.h"
#include <d3d11.h>
#include <EASTL/unique_ptr.h>

#include <SimpleMath.h>
namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

namespace SE_G {
    namespace Bind {
        class TransformCBuffer;
    }
}

class SUNSHINE_ENGINE_API TransformComponent :
    public Component
{
    friend class TransformComponent_Info;
public:
    TransformComponent();
    ~TransformComponent();

    TransformComponent(ID3D11Device* device);

    void SetupBuffer(ID3D11Device* device);

    eastl::unique_ptr<SE_G::Bind::TransformCBuffer> transformBuffer;

    void BindToGraphicsPipeline(ID3D11DeviceContext* context);

    // Transforms
    DXSM::Matrix GetTransalationMatrix() const;
    DXSM::Matrix GetRotationMatrix() const;
    DXSM::Matrix GetScaleMatrix() const;

    // Additional Transforms
    DXSM::Matrix GetLocalTransalationMatrix() const;
    DXSM::Matrix GetLocalRotationMatrix() const;
    DXSM::Matrix GetLocalScaleMatrix() const;

    DXSM::Matrix GetLocalTransformMatrix() const;
    DXSM::Matrix GetWorldMatrix_noLocal() const;

    // Full World Position
    bool m_isAbsoluteTransformCached = false;
    DXSM::Vector3 m_cachedAbsoluteWorldPosition;
    DXSM::Quaternion m_cachedAbsoluteWorldRotation_quat;
    DXSM::Vector3 m_cachedAbsoluteWorldRotation;
    void CalcAbsoluteTransform();

    DXSM::Vector3 GetAbsoluteWorldPosition();
    DXSM::Vector3 GetAbsoluteWorldRotation();
    DXSM::Quaternion GetAbsoluteWorldRotation_quat();

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

    DXSM::Vector2 m_uvMultiplier = { 1, 1 };

    DXSM::Matrix localTransfrom = DXSM::Matrix::Identity;

    const DXSM::Vector3& GetPosition() const { return m_position; }
    void SetPosition(const DXSM::Vector3& newPos) { m_position = newPos; }
    const DXSM::Vector3& GetRotation() const { return m_rotation; }
    void SetRotation(const DXSM::Vector3& newRot) { m_rotation = newRot; }
    const DXSM::Vector3& GetScaleFactor() const { return m_scaleFactor; }
    void SetScaleFactor(const DXSM::Vector3& newScaleFactor) { m_scaleFactor = newScaleFactor; }

    const DXSM::Vector3& GetLocalPosition() const { return m_localPosition; }
    void SetLocalPosition(const DXSM::Vector3& newPos) { m_localPosition = newPos; }
    const DXSM::Vector3& GetLocalRotation() const { return m_localRotation; }
    void SetLocalRotation(const DXSM::Vector3& newRot) { m_localRotation = newRot; }
    const DXSM::Vector3& GetLocalScaleFactor() const { return m_localScaleFactor; }
    void SetLocalScaleFactor(const DXSM::Vector3& newScaleFactor) { m_localScaleFactor = newScaleFactor; }

    const DXSM::Vector2& GetUVMultiplier() const { return m_uvMultiplier; }
    void SetUVMultiplier(const DXSM::Vector2& uvMultiplier) { m_uvMultiplier = uvMultiplier; }

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

    void SetParentTransform(TransformComponent* parentTransform);
    TransformComponent* GetParentTransform();

	void EnableMeshTransformMode();
	void DisableMeshTransformMode();
	bool IsMeshTransformMode();

    TransformComponent* m_parentTransform = nullptr;
private:

	bool m_meshTransformMode = false;
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

    void SetParentTransform(TransformComponent_Info* parentTransform_Info);
};

#ifndef TRANSFORMCOMPONENT_LUA_PROPERTIES_APPLY
#define TRANSFORMCOMPONENT_LUA_PROPERTIES_APPLY(FP) \
    FP(position, &TransformComponent::GetPosition, &TransformComponent::SetPosition), \
    FP(rotation, &TransformComponent::GetRotation, &TransformComponent::SetRotation), \
    FP(scaleFactor, &TransformComponent::GetScaleFactor, &TransformComponent::SetScaleFactor), \
    FP(localPosition, &TransformComponent::GetLocalPosition, &TransformComponent::SetLocalPosition), \
    FP(localRotation, &TransformComponent::GetLocalRotation, &TransformComponent::SetLocalRotation), \
    FP(localScaleFactor, &TransformComponent::GetLocalScaleFactor, &TransformComponent::SetLocalScaleFactor), \
    FP(uvMultiplier, &TransformComponent::GetUVMultiplier, &TransformComponent::SetUVMultiplier)
#endif

#ifndef TRANSFORMCOMPONENT_LUA_METHODS_APPLY
#define TRANSFORMCOMPONENT_LUA_METHODS_APPLY(FM) \
    FM("getAbsolutePosition", &TransformComponent::GetAbsoluteWorldPosition) \
    FM("getAbsoluteRotation", &TransformComponent::GetAbsoluteWorldRotation)
#endif