#include <Physics/GrabSystem.h>
#include <Physics/PhysicsSystem.h>

#include <GameObject/GameObject.h>

#include <Component/TransformComponent.h>
#include <Component/CharacterComponent.h>
#include <Component/CharacterControllerComponent.h>
#include <Component/GrabComponent.h>
#include <Component/PhysicsComponent.h>

#include <Scene.h>

#include <EASTL/vector.h>


GrabSystem::GrabSystem()
{
	m_grabPairs = eastl::unordered_map<SE::UUID, GrabRuntime>();
}

bool GrabSystem::HasGrabPair(SE::UUID character)
{
	return m_grabPairs.contains(character);
}

void GrabSystem::Update(float deltaTime)
{
	UpdateGrabTargets(deltaTime);
}

void GrabSystem::ProcessGrabInput(GameObject* gameObj)
{
	eastl::shared_ptr<GrabComponent> grabComp = gameObj->GetComponent<GrabComponent>();
	eastl::shared_ptr<CharacterComponent> charComp = gameObj->GetComponent<CharacterComponent>();
	eastl::shared_ptr<CharacterControllerComponent> controller = gameObj->GetComponent<CharacterControllerComponent>();
	eastl::shared_ptr<TransformComponent> trComp = gameObj->GetComponent<TransformComponent>();

	auto viewPos = trComp->GetAbsoluteWorldPosition();
	float charYaw = charComp->m_yaw;
	auto viewDir = JPH::RVec3(sin(charYaw), 0.0f, cos(charYaw));

	eastl::vector<SE::UUID> ignoreObjects = eastl::vector<SE::UUID>(1, gameObj->m_UUID);

	SE::UUID hitUUID = SE::UUID(0u);
	bool hitSMTH = m_systemContext.physics->RayCast(JPH::RVec3(viewPos.x, viewPos.y, viewPos.z),
		viewDir,
		grabComp->m_maxGrabDistance, ignoreObjects, &hitUUID);

	if (!hitSMTH || hitUUID == SE::UUID(0u)) {
		return;
	}

	auto hitObj = m_systemContext.scene->GetGameObjectByUUID(hitUUID);
	auto physComp = hitObj->GetComponent<PhysicsComponent>();

	if (physComp->GetMotionType() == JPH::EMotionType::Dynamic && grabComp->m_canGrabDynamicBodies)
	{
		GrabRuntime grabRt;
		grabRt.m_character = gameObj->m_UUID;
		grabRt.m_grabbedObject = hitUUID;
		
		m_grabPairs[gameObj->m_UUID] = grabRt;

		// other data will be calculated in grabSystem update
	}
	// for now only dynamic items can be grabbed
}

void GrabSystem::UpdateGrabTargets(float deltaTime)
{
	for (auto grabPair : m_grabPairs)
	{
		auto charUUID = grabPair.first;
		auto charObj = m_systemContext.scene->GetGameObjectByUUID(charUUID);

		auto grabObjUUID = grabPair.second.m_grabbedObject;
		auto grabObj = m_systemContext.scene->GetGameObjectByUUID(grabObjUUID);

		auto grabComp = charObj->GetComponent<GrabComponent>();
		if (!grabComp) { continue; }
		auto character = charObj->GetComponent<CharacterComponent>();
		if (!character) { continue; }
		auto charTransform = charObj->GetComponent<TransformComponent>();
		if (!charTransform) { continue; }
		auto grabTransform = grabObj->GetComponent<TransformComponent>();
		if (!grabTransform) { continue; }

		auto forwardDir = DXSM::Vector3(sin(character->m_yaw), 0, cos(character->m_yaw));
		forwardDir *= grabComp->m_holdDistance;

		grabPair.second.m_targetPosition = charTransform->GetAbsoluteWorldPosition() + forwardDir;
		grabPair.second.m_localGrabOffset = grabPair.second.m_targetPosition - grabTransform->GetAbsoluteWorldPosition();

		auto grabPhysics = grabObj->GetComponent<PhysicsComponent>();
		if (!grabPhysics) { continue; }

		DXSM::Vector3 force = grabPair.second.m_localGrabOffset * grabPair.second.m_springStrength
			- grabPhysics->GetLinearVelocity() * grabPair.second.m_damping;

		grabPhysics->AddImpulse(force);
	}
}

void GrabSystem::ProcessRelease()
{

}

void GrabSystem::ProcessThrow()
{

}
