#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Constraints/Constraint.h>

#include <SimpleMath.h>
#include <Utils/UUID.h>

namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

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

    virtual Type GetType() const = 0;

protected:

    JPH::Ref<JPH::Constraint> m_constraint;
};
