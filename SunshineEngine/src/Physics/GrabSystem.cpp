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
	m_grabInputQueue = eastl::vector<GrabInput>();
}

bool GrabSystem::HasGrabPair(SE::UUID character)
{
	return m_grabPairs.contains(character);
}

void GrabSystem::EnqueueGrabInput(GrabInput grabInput)
{
	m_grabInputQueue.push_back(grabInput);
}

void GrabSystem::FlushGrabQueue()
{
	for (auto grabInput : m_grabInputQueue)
	{
		switch (grabInput.action)
		{
		case GrabInput::Grab:
			ProcessGrab(grabInput.gameObject);
			break;
		case GrabInput::Release:
			ProcessRelease(grabInput.gameObject);
			break;
		case GrabInput::Throw:
			ProcessThrow(grabInput.gameObject);
			break;
		default:
			break;
		}

	}
	m_grabInputQueue.clear();
}

void GrabSystem::Update(float deltaTime)
{
	FlushGrabQueue();
	UpdateGrabTargets(deltaTime);
}

void GrabSystem::ProcessGrab(GameObject* gameObj)
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

		grabComp->m_grabbedObject = hitUUID;

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

		DXSM::Vector3 force = grabPair.second.m_localGrabOffset * grabComp->m_grabSpringStrength
			- grabPhysics->GetLinearVelocity() * grabComp->m_grabSpringDamping;

		grabPhysics->AddForce(force);
	}
}

void GrabSystem::ProcessRelease(GameObject* gameObj)
{
	auto res = m_grabPairs.find(gameObj->m_UUID);
	if (res == m_grabPairs.end()) { return; }
	m_grabPairs.erase(res);

	eastl::shared_ptr<GrabComponent> grabComp = gameObj->GetComponent<GrabComponent>();
	if (!grabComp) { return; }

	grabComp->m_grabbedObject = SE::UUID(0u);
}

void GrabSystem::ProcessThrow(GameObject* gameObj)
{
	auto res = m_grabPairs.find(gameObj->m_UUID);
	if (res == m_grabPairs.end()) {
		return;
	}

	auto grabRt = res->second;
	m_grabPairs.erase(res);

	eastl::shared_ptr<GrabComponent> grabComp = gameObj->GetComponent<GrabComponent>();
	if (!grabComp) { return; }
	eastl::shared_ptr<CharacterComponent> charComp = gameObj->GetComponent<CharacterComponent>();
	if (!charComp) { return; }

	auto grabObjUUID = grabRt.m_grabbedObject;
	auto grabObj = m_systemContext.scene->GetGameObjectByUUID(grabObjUUID);

	auto forwardDir = DXSM::Vector3(sin(charComp->m_yaw), 0, cos(charComp->m_yaw));

	auto grabPhysics = grabObj->GetComponent<PhysicsComponent>();
	if (!grabPhysics) { return; }

	DXSM::Vector3 impulse = grabComp->m_throwImpulse * forwardDir;

	grabPhysics->AddImpulse(impulse);

	grabComp->m_grabbedObject = SE::UUID(0u);
}
