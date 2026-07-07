#include <Physics/PhysicsSystem.h>

#include <Component/CharacterControllerComponent.h>
#include <Component/TransformComponent.h>
#include <Component/RenderComponent.h>

#include <Graphics/Renderer/Technique/ColliderTechnique.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

CharacterControllerComponent::CharacterControllerComponent()
{
	m_colliderData = eastl::make_shared<SE::ColliderData>(SE::ColliderShapeType::Capsule);
	m_colliderData->m_settings.data.asCapsule.m_radius = 0.35f;
	m_colliderData->m_settings.data.asCapsule.m_height = 1.8f;
}

CharacterControllerComponent::~CharacterControllerComponent()
{
	DestroyCharacter();
}

CharacterControllerComponent::CharacterControllerComponent(PhysicsSystem* physicsSystem,
	TransformComponent* transformComp, SE::UUID uuid)
{
	Initialize(physicsSystem, transformComp, uuid);
}

void CharacterControllerComponent::Initialize(PhysicsSystem* physicsSystem,
	TransformComponent* transformComp, SE::UUID uuid)
{
	m_uuid = uuid;

	JPH::PhysicsSystem& physSystem = physicsSystem->GetWorld();

	float Radius = m_colliderData->m_settings.data.asCapsule.m_radius;
	float Height = m_colliderData->m_settings.data.asCapsule.m_height;

	m_shape = new JPH::CapsuleShape(
		//Height * 0.5f - Radius,
		Height * 0.5f,
		Radius
	);

	JPH::CharacterVirtualSettings settings;
	settings.mShape = m_shape;
	settings.mMaxSlopeAngle =
		JPH::DegreesToRadians(
			m_maxSlopeAngle
		);
	settings.mMaxStrength = 10000.0f;
	settings.mCharacterPadding = 0.02f;
	settings.mPenetrationRecoverySpeed = 1.0f;
	settings.mInnerBodyShape = m_shape;
	settings.mInnerBodyLayer = SE::Layers::CHARACTER;

	DXSM::Quaternion _quat = transformComp->GetAbsoluteWorldRotation_quat();
	const JPH::Quat targetRot(_quat.x, _quat.y, _quat.z, _quat.w);

	auto temp = transformComp->GetPosition();
	m_character = new JPH::CharacterVirtual(&settings,
		JPH::RVec3Arg(temp.x, temp.y, temp.z),
		targetRot, m_uuid.m_UUID, &physSystem);


	Initialized = true;
}

void CharacterControllerComponent::DestroyCharacter()
{
	Initialized = false;

	m_character = nullptr;
	m_shape = nullptr;

	m_velocity = DXSM::Vector3::Zero;

	m_grounded = false;

	m_groundNormal = DXSM::Vector3(0.0f, 1.0f, 0.0f);
}

CharacterControllerComponent_Info::CharacterControllerComponent_Info(
	RenderComponent_Info* rc_info,
	TransformComponent_Info* tc_info)
	: m_rc_info(rc_info)
{
	m_assignedComponent = eastl::make_unique<CharacterControllerComponent>();

	// Init collider
	auto device = rc_info->m_assignedComponent.get()->GetDevice();
	auto colliderTech = eastl::make_unique<SE_G::ColliderTechnique>(
		device, tc_info->m_assignedComponent.get(), eastl::string("ColliderPass"),
		m_assignedComponent->m_colliderData);

	rc_info->AddTechnique(eastl::move(colliderTech));

	m_isValid = true;
}

CharacterControllerComponent_Info::~CharacterControllerComponent_Info() {
	if (m_isValid)
		m_rc_info->RemoveTechnique("ColliderPass");
}

#define CHARCONTR_ADD_PROPERTIES(name, getter, setter) #name, sol::property(getter, setter)
#define CHARCONTR_PROPERTIES_PAIRS CHARACTERCONTROLLER_LUA_PROPERTIES_APPLY(CHARCONTR_ADD_PROPERTIES)

#define CHARCONTR_METHOD_PAIRS

LUA_REGISTER_COMPONENT(
	CharacterControllerComponent,
	"CharacterControllerComponent",
	/* no fields */,
	CHARCONTR_PROPERTIES_PAIRS,
	CHARCONTR_METHOD_PAIRS,
	"getCharacterController")

#undef CHARCONTR_ADD_PROPERTIES
#undef CHARCONTR_METHOD_PAIRS