#pragma once

#include <EASTL/unordered_map.h>
#include <EASTL/shared_ptr.h>

#include <Utils/UUID.h>

class PhysicsConstraint;

class GrabRuntime
{
public:

    SE::UUID Character;

    SE::UUID GrabbedObject;

    eastl::shared_ptr<PhysicsConstraint> Constraint;

    SE::UUID TargetAnchor;

    bool IsThrowRequested = false;
};

class GrabSystem
{
public:
    void Update();

    void ProcessGrabInput();
    void UpdateGrabTargets();
    void UpdateConstraints();
    void ProcessRelease();
    void ProcessThrow();

    eastl::unordered_map<SE::UUID, GrabRuntime> m_grabPairs;
};