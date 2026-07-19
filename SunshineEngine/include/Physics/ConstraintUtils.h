#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Constraints/Constraint.h>

#include <EASTL/shared_ptr.h>

#include <Utils/UUID.h>
#include <SimpleMath.h>

namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

/*
// Low level (game engine)

struct PhysicsConstraintDesc
{
    PhysicsConstraint::Type Type;

    SE::UUID BodyA;
    SE::UUID BodyB;

    DXSM::Vector3 AnchorA;
    DXSM::Vector3 AnchorB;

    DXSM::Quaternion RotationA;
    DXSM::Quaternion RotationB;

    bool EnableCollision = false;

    // SpringSettings Spring;

    // LimitSettings Limits;
};

class PhysicsConstraint
{
public:

    enum class Type
    {
        Fixed,
        Distance,
        Point,
        Hinge,
        SixDOF
    };

    virtual ~PhysicsConstraint() = default;

    Type GetType() const;

private:

    friend class PhysicsSystem;

    Type m_type;

    JPH::Ref<JPH::Constraint> m_constraint;
};

// High level (game design)

class PhysicsGrabHandle
{
public:

    SE::UUID m_grabbedBody;

    // eastl::shared_ptr<PhysicsConstraint> m_constraint;

    DXSM::Vector3 m_targetPosition;

    DXSM::Quaternion m_targetRotation;

    void SetTargetPosition(
        const DXSM::Vector3& position);

    void SetTargetRotation(
        const DXSM::Quaternion& rotation);

    void Release();
};

*/

