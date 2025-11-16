#include "Component/TransformComponent.h"
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

TransformComponent::TransformComponent(ID3D11Device* device) {
    SetupBuffer(device);
}

void TransformComponent::SetupBuffer(ID3D11Device* device)
{
    transformBuffer = new SE_G::Bind::TransformCBuffer(device, this, 0u);
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
    return DXSM::Matrix::CreateFromYawPitchRoll(m_localRotation.y, m_localRotation.x, m_localRotation.z);
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
    return DXSM::Matrix::CreateFromYawPitchRoll(m_rotation.y, m_rotation.x, m_rotation.z);
}

DXSM::Matrix TransformComponent::GetScaleMatrix() const
{
    return DXSM::Matrix::CreateScale(m_scaleFactor);
}

DXSM::Matrix TransformComponent::GetWorldMatrix() const
{
    return GetLocalTransformMatrix() * GetScaleMatrix() * GetRotationMatrix() * GetTransalationMatrix();
}

#define TC_ADD_FIELD(name) #name, &TransformComponent::name
#define TC_FIELD_PAIRS TRANSFORMCOMPONENT_LUA_FIELDS_APPLY(TC_ADD_FIELD)
#define TC_METHOD_PAIRS
LUA_REGISTER_COMPONENT(TransformComponent, "TransformComponent", TC_FIELD_PAIRS, TC_METHOD_PAIRS, "getTransform")
#undef TC_ADD_FIELD
