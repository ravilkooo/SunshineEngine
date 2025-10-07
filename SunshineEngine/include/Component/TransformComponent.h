#pragma once
#include "SunshineEngineAPI.h"
#include "Component.h"
#include <SimpleMath.h>

namespace DXSM = DirectX::SimpleMath;

class SUNSHINE_ENGINE_API TransformComponent :
    public Component
{
public:
    TransformComponent() {};

    ~TransformComponent() = default;
    /*
    // ================
    // To make SUNSHINE_ENGINE_API work

    TransformComponent(TransformComponent&&) noexcept = default;
    TransformComponent& operator=(TransformComponent&&) noexcept = default;

    TransformComponent(const TransformComponent&) = delete;
    TransformComponent& operator=(const TransformComponent&) = delete;
    // ================
    */

    DXSM::Vector3 position = { 0, 0, 0 };
    DXSM::Vector3 rotation = { 0, 0, 0 }; // Pitch (x-axis), Yaw (y-axis), Roll (z-axis)
    DXSM::Vector3 scaleFactor = { 1, 1, 1 };
    DXSM::Matrix localTransfrom = DXSM::Matrix::Identity;
    
    const std::type_info& getType() const override {
        return typeid(TransformComponent);
    }
};