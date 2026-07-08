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
#include <Graphics/Renderer/Technique/ColliderTechnique.h>

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
    transformComp = tc;

    SetObjecUUID(objectUUID);
}

TriggerComponent::~TriggerComponent()
{
    if (m_physicsSystem)
        m_physicsSystem->RemoveTrigger(this);
}

void TriggerComponent::SetLuaCallback(sol::function callback)
{
    m_luaCallback = std::move(callback);
}

void TriggerComponent::OnEnter(SE::UUID otherUUID)
{
    if (otherUUID == m_objectUUID)
        return;
    if (m_insideObjects.find(otherUUID) != m_insideObjects.end())
    {
        return;  // Already inside, don't trigger again
    }

    m_insideObjects.insert(otherUUID);

    if (m_luaCallback.valid())
    {
        try
        {
            m_luaCallback("enter", otherUUID.GetHilo());
        }
        catch (const std::exception& e)
        {
            printf("[TriggerComponent::OnEnter] Lua error: %s", e.what());
        }
    }
}

void TriggerComponent::OnExit(SE::UUID otherUUID)
{
    if (otherUUID == m_objectUUID)
        return;
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
            m_luaCallback("exit", otherUUID.GetHilo());
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

void TriggerComponent::InitTransforms()
{
    transformComp->CalcAbsoluteTransform();

    auto pos = transformComp->GetAbsoluteWorldPosition();
    auto quat = transformComp->GetAbsoluteWorldRotation_quat();

    m_position.Set(pos.x, pos.y, pos.z);
    m_orientation.Set(quat.x, quat.y, quat.z, quat.w);
}

JPH::BodyID TriggerComponent::GetBodyID() const
{
    return m_joltBodyId;
}

void TriggerComponent::FromJson(const json& j)
{
    // Collider/shape data
    if (j.contains("collider") && j["collider"].is_object()) {
        m_colliderData.FromJson(j["collider"]);
    }
    else
    {
        m_colliderData = SE::ColliderData(SE::ColliderShapeType::Box);
    }
    SE::PhysicsMotionType motion = SE::PhysicsMotionType::Static;
    if (j.contains("m_motion"))
    {
        j.at("m_motion").get_to(motion);
    }
    switch (motion) {
    case SE::PhysicsMotionType::Static:    m_triggerMotionType = JPH::EMotionType::Static; break;
    case SE::PhysicsMotionType::Kinematic: m_triggerMotionType = JPH::EMotionType::Kinematic; break;
    default: m_triggerMotionType = JPH::EMotionType::Static; break;
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
    j["m_motion"] = m_triggerMotionType;
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
    if (j.contains("m_motion")) j.at("m_motion").get_to(m_triggerMotionType);
    m_colliderData->m_settings.colliderColor = DXSM::Vector3(
        1.0f,
        1.0f,
        0.0f
    );
}

#define TRIGGER_ADD_METHOD(k, fn) k, fn

LUA_REGISTER_COMPONENT(
    TriggerComponent,
    "TriggerComponent",
    /* no fields */,
    /* no properties */,
    TRIGGER_LUA_METHODS_APPLY(TRIGGER_ADD_METHOD),
    "getTrigger")
#undef TRIGGER_ADD_METHOD
