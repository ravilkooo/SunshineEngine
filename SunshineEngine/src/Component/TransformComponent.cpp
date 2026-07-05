#include <Component/TransformComponent.h>
#include <Graphics/Bindable/TransformCBuffer.h>
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

TransformComponent::TransformComponent() {
}

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

const DXSM::Vector3& TransformComponent::GetPosition() const
{
    return m_position;
}
void TransformComponent::SetPosition(DXSM::Vector3 newPos)
{
    m_position = newPos;
}
const DXSM::Vector3& TransformComponent::GetRotation() const
{
    return m_rotation;
}
void TransformComponent::SetRotation(DXSM::Vector3 newRot)
{
    m_rotation = newRot;
}
const DXSM::Vector3& TransformComponent::GetScaleFactor() const
{
    return m_scaleFactor;
}
void TransformComponent::SetScaleFactor(DXSM::Vector3 newScaleFactor)
{
    m_scaleFactor = newScaleFactor;
}

const DXSM::Vector3& TransformComponent::GetLocalPosition() const
{
    return m_localPosition;
}
void TransformComponent::SetLocalPosition(DXSM::Vector3 newPos)
{
    m_localPosition = newPos;
}
const DXSM::Vector3& TransformComponent::GetLocalRotation() const
{
    return m_localRotation;
}
void TransformComponent::SetLocalRotation(DXSM::Vector3 newRot)
{
    m_localRotation = newRot;
}
const DXSM::Vector3& TransformComponent::GetLocalScaleFactor() const
{
    return m_localScaleFactor;
}
void TransformComponent::SetLocalScaleFactor(DXSM::Vector3 newScaleFactor)
{
    m_localScaleFactor = newScaleFactor;
}

const DXSM::Vector2& TransformComponent::GetUVMultiplier() const
{
    return m_uvMultiplier;
}
void TransformComponent::SetUVMultiplier(DXSM::Vector2 uvMultiplier)
{
    m_uvMultiplier = uvMultiplier;
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
    DXSM::Matrix wt = GetScaleMatrix() * GetRotationMatrix() * GetTransalationMatrix();

    if (m_parentTransform)
    {
        wt = wt * m_parentTransform->GetWorldMatrix_noLocal();
    }
    return wt;
}

void TransformComponent::CalcAbsoluteTransform()
{
    if (!m_isAbsoluteTransformCached)
    {
        auto wMat = GetWorldMatrix_noLocal();

        DX::XMVECTOR scale, rotation, translation;
        DX::XMMatrixDecompose(&scale, &rotation, &translation, DX::XMLoadFloat4x4(&wMat));

        DX::XMStoreFloat3(&m_cachedAbsoluteWorldPosition, translation);
        DX::XMStoreFloat4(&m_cachedAbsoluteWorldRotation_quat, rotation);

        m_cachedAbsoluteWorldRotation = m_cachedAbsoluteWorldRotation_quat.ToEuler();
        m_isAbsoluteTransformCached = true;
    }
}

DXSM::Vector3 TransformComponent::GetAbsoluteWorldPosition()
{
    CalcAbsoluteTransform();
    return m_cachedAbsoluteWorldPosition;
}

DXSM::Quaternion TransformComponent::GetAbsoluteWorldRotation_quat()
{
    CalcAbsoluteTransform();
    return m_cachedAbsoluteWorldRotation_quat;
}

DXSM::Vector3 TransformComponent::GetAbsoluteWorldRotation()
{
    CalcAbsoluteTransform();
    return m_cachedAbsoluteWorldRotation;
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
        wt = wt * m_parentTransform->GetWorldMatrix_noLocal();
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
        currNode = currNode->GetParentTransform();
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

#define TC_ADD_PROPERTY(name, getter, setter) #name, sol::property(getter, setter)
#define TC_PROPERTY_PAIRS TRANSFORMCOMPONENT_LUA_PROPERTIES_APPLY(TC_ADD_PROPERTY)

#define TC_ADD_METHOD_WITH_LEAD(k, fn) , k, fn
#define TC_METHOD_PAIRS TRANSFORMCOMPONENT_LUA_METHODS_APPLY(TC_ADD_METHOD_WITH_LEAD)

LUA_REGISTER_COMPONENT(
    TransformComponent,
    "TransformComponent",
    /* no fields */,
    TC_PROPERTY_PAIRS,
    TC_METHOD_PAIRS,
    "getTransform")

#undef TC_ADD_PROPERTY
#undef TC_ADD_METHOD_WITH_LEAD
