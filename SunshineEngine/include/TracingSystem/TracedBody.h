#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include <Utils/UUID.h>

#include <Component/TransformComponent.h>

class TracingSystem;

class TracedBody
{
	friend class TracingSystem;
public:
	TracedBody() = default;
	TracedBody(SE::UUID objectUUID, TransformComponent* tc)
		: m_objectUUID(objectUUID), transformComp(tc)
	{
		transformComp = tc;

		m_position.Set(tc->m_position.x, tc->m_position.y, tc->m_position.z);
		auto quat = DXSM::Quaternion::CreateFromYawPitchRoll(tc->m_rotation.y, tc->m_rotation.x, tc->m_rotation.z);
		m_orientation.Set(quat.x, quat.y, quat.z, quat.w);
	}

	TransformComponent* transformComp;

	SE::UUID m_objectUUID;

	JPH::RVec3 m_position;
	JPH::Quat m_orientation;
	JPH::EMotionType m_motionType;
	JPH::EActivation m_activation;
	JPH::ObjectLayer m_objectLayer;
	JPH::ShapeRefC m_shape;


	JPH::Body* m_joltBody;
	JPH::BodyID m_joltBodyId;
};