#include <Physics/PhysicsSystem.h>

#include <Component/TransformComponent.h>
#include <Component/CharacterControllerComponent.h>

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

	Shape = new JPH::CapsuleShape(
		Height * 0.5f - Radius,
		Radius
	);

	JPH::CharacterVirtualSettings settings;
	settings.mShape = Shape;
	settings.mMaxSlopeAngle =
		JPH::DegreesToRadians(
			MaxSlopeAngle
		);
	settings.mMaxStrength = 100.0f;
	settings.mCharacterPadding = 0.02f;
	settings.mPenetrationRecoverySpeed = 1.0f;
	settings.mInnerBodyLayer = SE::Layers::MOVING;

	DXSM::Quaternion _quat = transformComp->GetAbsoluteWorldRotation_quat();
	const JPH::Quat targetRot(_quat.x, _quat.y, _quat.z, _quat.w);

	Character = new JPH::CharacterVirtual(&settings,
		JPH::RVec3(
			transformComp->m_position.x,
			transformComp->m_position.y,
			transformComp->m_position.z),
		targetRot, m_uuid.m_UUID, &physSystem);

	Initialized = true;
}


void CharacterControllerComponent::DestroyCharacter()
{
	Initialized = false;

	Character = nullptr;
	Shape = nullptr;

	Velocity = DXSM::Vector3::Zero;

	Grounded = false;

	GroundNormal = DXSM::Vector3(0.0f, 1.0f, 0.0f);
}