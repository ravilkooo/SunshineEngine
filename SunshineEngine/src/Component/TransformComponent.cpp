#include "Component/TransformComponent.h"

TransformComponent::TransformComponent(ID3D11Device* device) {
    SetupBuffer(device);
}

void TransformComponent::SetupBuffer(ID3D11Device* device)
{
    transformBuffer = new Bind::TransformCBuffer(device, this, 0u);
}

void TransformComponent::BindToGraphicsPipeline(ID3D11DeviceContext* context) {
    transformBuffer->Bind(context);
}

DXSM::Matrix TransformComponent::GetLocalTransalationMatrix() const
{
    return DXSM::Matrix::CreateTranslation(m_localPosition);
}

DXSM::Matrix TransformComponent::GetLocalRotationMatrix() const
{
    return DXSM::Matrix::CreateFromYawPitchRoll(m_localRotation);
}

DXSM::Matrix TransformComponent::GetLocalScaleMatrix() const
{
    return DXSM::Matrix::CreateScale(m_localScaleFactor);
}

DXSM::Matrix TransformComponent::GetLocalTransformMatrix() const
{
    return GetLocalScaleMatrix() * GetLocalRotationMatrix() * GetLocalTransalationMatrix();
}


DXSM::Matrix TransformComponent::GetTransalationMatrix() const
{
    return DXSM::Matrix::CreateTranslation(m_position);
}

DXSM::Matrix TransformComponent::GetRotationMatrix() const
{
    return DXSM::Matrix::CreateFromYawPitchRoll(m_rotation);
}

DXSM::Matrix TransformComponent::GetScaleMatrix() const
{
    return DXSM::Matrix::CreateScale(m_scaleFactor);
}

DXSM::Matrix TransformComponent::GetWorldMatrix() const
{
    return GetLocalTransformMatrix() * GetScaleMatrix() * GetLocalRotationMatrix() * GetTransalationMatrix();
}
