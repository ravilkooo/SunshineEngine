#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapSuleShape.h>
#include <Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h>

#include <Component/TriggerComponent.h>
#include <Component/TransformComponent.h>
#include <Component/RenderComponent.h>

#include <Physics/PhysicsSystem.h>

#include <Graphics/Renderer/DeferredRenderer.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

#include <SimpleMath.h>

TriggerComponent::TriggerComponent()
    : m_luaCallback(sol::nil)
{
}

TriggerComponent::TriggerComponent(SE::UUID objectUUID, TransformComponent* tc)
    : m_luaCallback(sol::nil)
{
    SetObjecUUID(objectUUID);
    InitTransforms(tc);
}

TriggerComponent::~TriggerComponent()
{
    m_physicsSystem->RemoveTrigger(this);
}

void TriggerComponent::SetLuaCallback(sol::function callback)
{
    m_luaCallback = std::move(callback);
}

void TriggerComponent::OnEnter(SE::UUID otherUUID)
{
    if (m_insideObjects.find(otherUUID) != m_insideObjects.end())
    {
        return;  // Already inside, don't trigger again
    }

    m_insideObjects.insert(otherUUID);

    if (m_luaCallback.valid())
    {
        try
        {
            m_luaCallback("enter", otherUUID.m_UUID);
        }
        catch (const std::exception& e)
        {
            printf("[TriggerComponent::OnEnter] Lua error: %s", e.what());
        }
    }
}

void TriggerComponent::OnExit(SE::UUID otherUUID)
{
    auto it = m_insideObjects.find(otherUUID);
    if (it == m_insideObjects.end())
    {
        return;  // Not inside, nothing to exit
    }

    m_insideObjects.erase(it);

    if (m_luaCallback.valid())
    {
        try
        {
            m_luaCallback("exit", otherUUID.m_UUID);
        }
        catch (const std::exception& e)
        {
            printf("[TriggerComponent::OnExit] Lua error: %s", e.what());
        }
    }
}

void TriggerComponent::SetObjecUUID(SE::UUID objectUUID)
{
    m_objectUUID = objectUUID;
}

void TriggerComponent::SetShape(JPH::ShapeRefC shapePtr)
{
    m_shape = shapePtr;
}

void TriggerComponent::InitTransforms(TransformComponent* tc)
{
    transformComp = tc;

    auto wMat = tc->GetWorldMatrix_noLocal();

    DX::XMVECTOR scale, rotation, translation;
    DX::XMMatrixDecompose(&scale, &rotation, &translation, DX::XMLoadFloat4x4(&wMat));

    DXSM::Vector3 _pos;
    DXSM::Quaternion _quat;

    DX::XMStoreFloat3(&_pos, translation);
    DX::XMStoreFloat4(&_quat, rotation);

    m_position.Set(_pos.x, _pos.y, _pos.z);
    m_orientation.Set(_quat.x, _quat.y, _quat.z, _quat.w);
}

JPH::Body* TriggerComponent::GetBody() const
{
    return m_joltBody;
}

JPH::BodyID TriggerComponent::GetBodyID() const
{
    return m_joltBodyId;
}

void TriggerComponent::FromJson(const json& j)
{
    SE::ColliderData colliderData;
    // Collider/shape data
    if (j.contains("collider") && j["collider"].is_object()) {
        colliderData.FromJson(j["collider"]);
    }
    else
    {
        colliderData = SE::ColliderData(SE::ColliderShapeType::Box);
    }

    JPH::ShapeSettings::ShapeResult shapeResult;
    switch (colliderData.m_shapeType) {
    case SE::ColliderShapeType::Box: {
        JPH::BoxShapeSettings boxSettings(
            JPH::Vec3(
                colliderData.m_settings.data.asBox.m_size.x * 0.5f,
                colliderData.m_settings.data.asBox.m_size.y * 0.5f,
                colliderData.m_settings.data.asBox.m_size.z * 0.5f
            )
        );
        shapeResult = boxSettings.Create();
        break;
    }
    case SE::ColliderShapeType::Sphere: {
        JPH::SphereShapeSettings sphereSettings(colliderData.m_settings.data.asSphere.m_radius);
        shapeResult = sphereSettings.Create();
        break;
    }
    case SE::ColliderShapeType::Capsule: {
        JPH::CapsuleShapeSettings capsuleSettings(
            colliderData.m_settings.data.asCapsule.m_height * 0.5f,
            colliderData.m_settings.data.asCapsule.m_radius
        );
        shapeResult = capsuleSettings.Create();
        break;
    }
    case SE::ColliderShapeType::TaperedCapsule: {
        JPH::TaperedCapsuleShapeSettings taperedCapsuleSettings(
            colliderData.m_settings.data.asTaperedCapsule.m_height * 0.5f,
            colliderData.m_settings.data.asTaperedCapsule.m_topRadius,
            colliderData.m_settings.data.asTaperedCapsule.m_bottomRadius
        );
        shapeResult = taperedCapsuleSettings.Create();
        break;
    }
    default:
        // Fallback to box if shape type is not recognized
        JPH::BoxShapeSettings defaultBoxSettings(JPH::Vec3(0.5f, 0.5f, 0.5f));
        shapeResult = defaultBoxSettings.Create();
        break;
    }

    if (shapeResult.IsValid()) {
        m_shape = shapeResult.Get();
    }

}

TriggerComponent_Info::TriggerComponent_Info(
    RenderComponent_Info* rc_info,
    TransformComponent_Info* tc_info)
    : m_rc_info(rc_info)
{
    m_colliderData = eastl::make_shared<SE::ColliderData>(SE::ColliderShapeType::Box);

    // Init collider
    auto device = rc_info->m_assignedComponent.get()->GetDevice();
    auto colliderTech = eastl::make_unique<SE_G::ColliderTechnique>(
        device, tc_info->m_assignedComponent.get(), eastl::string("TriggerPass"),
        m_colliderData);

    rc_info->AddTechnique(eastl::move(colliderTech));

    SetShape(SE::ColliderShapeType::Box);
    SE::ColliderSettings collSettings{};
    collSettings.colliderColor = DXSM::Vector3(
        1.0f,
        1.0f,
        0.0f
    );
    collSettings.data.asBox = { { 4.0f, 2.0f, 4.0f } };
    m_colliderData->SetColliderSettings(collSettings);

    m_isValid = true;
}

TriggerComponent_Info::~TriggerComponent_Info() {
    if (m_isValid)
        m_rc_info->RemoveTechnique("TriggerPass");
}

SE::ColliderShapeType TriggerComponent_Info::GetShape() const
{
    return m_colliderData->m_shapeType;
}

void TriggerComponent_Info::SetShape(SE::ColliderShapeType shape)
{
    m_colliderData->SetShapeType(shape);
}

json TriggerComponent_Info::ToJson() const
{
    json j = json::object();
    if (m_colliderData) {
        j["collider"] = m_colliderData->ToJson();
    }
    return j;
}

void TriggerComponent_Info::FromJson(const json& j)
{
    // Collider/shape data
    if (j.contains("collider") && j["collider"].is_object()) {
        if (!m_colliderData) {
            m_colliderData = eastl::make_shared<SE::ColliderData>(SE::ColliderShapeType::Box);
        }
        m_colliderData->FromJson(j["collider"]);
    }
    m_colliderData->m_settings.colliderColor = DXSM::Vector3(
        1.0f,
        1.0f,
        0.0f
    );
}
