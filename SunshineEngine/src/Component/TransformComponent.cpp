#include "Component/TransformComponent.h"
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

TransformComponent::TransformComponent(ID3D11Device* device) {
    SetupBuffer(device);
}

TransformComponent::~TransformComponent() {
}

void TransformComponent::SetupBuffer(ID3D11Device* device)
{
    transformBuffer = eastl::make_unique<SE_G::Bind::TransformCBuffer>(device, this, 0u);
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

DXSM::Matrix TransformComponent::GetWorldMatrix_noLocal() const
{
    return GetScaleMatrix() * GetRotationMatrix() * GetTransalationMatrix();
}

DXSM::Matrix TransformComponent::GetWorldMatrix() const
{
    DXSM::Matrix wt = GetScaleMatrix() * GetRotationMatrix() * GetTransalationMatrix();

    if (m_meshTransformMode)
    {
		wt = GetLocalTransformMatrix() * wt;
    }

    if (m_parentTransform)
    {
        wt = wt * m_parentTransform->GetWorldMatrix();
    }
    return wt;
}

void TransformComponent::SetParentTransform(TransformComponent* parentTransform)
{
    TransformComponent* currNode = parentTransform;
    while (currNode)
    {
        if (currNode == this)
        {
            printf("Cyclce transform dependence prevented!\n");
            return;
        }
        currNode = parentTransform->GetParentTransform();
    }
    m_parentTransform = parentTransform;
}

TransformComponent* TransformComponent::GetParentTransform()
{
    return m_parentTransform;
}

void TransformComponent::EnableMeshTransformMode()
{
    m_meshTransformMode = true;
}

void TransformComponent::DisableMeshTransformMode()
{
    m_meshTransformMode = false;
}

bool TransformComponent::IsMeshTransformMode()
{
    return m_meshTransformMode;
}

TransformComponent_Info::TransformComponent_Info(ID3D11Device* device)
{
    m_assignedComponent = eastl::make_unique<TransformComponent>(device);
}

TransformComponent_Info::~TransformComponent_Info()
{

}

void TransformComponent_Info::SetParentTransform(TransformComponent_Info* parentTransform_Info)
{
    if (parentTransform_Info)
        m_assignedComponent->SetParentTransform(parentTransform_Info->m_assignedComponent.get());
    else
        m_assignedComponent->SetParentTransform(nullptr);
}

#define TC_ADD_FIELD(name) #name, &TransformComponent::name
#define TC_FIELD_PAIRS TRANSFORMCOMPONENT_LUA_FIELDS_APPLY(TC_ADD_FIELD)
#define TC_METHOD_PAIRS
LUA_REGISTER_COMPONENT(TransformComponent, "TransformComponent", TC_FIELD_PAIRS, TC_METHOD_PAIRS, "getTransform")
#undef TC_ADD_FIELD
