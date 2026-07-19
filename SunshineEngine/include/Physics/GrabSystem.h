#pragma once

#include <EASTL/unordered_map.h>
#include <EASTL/shared_ptr.h>

#include <Utils/UUID.h>
#include <SystemContext.h>

#include <SimpleMath.h>

namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

class GrabComponent;
class CharacterComponent;
class GameObject;

class GrabRuntime
{
public:

    SE::UUID m_character;
    SE::UUID m_grabbedObject;

    // Default values

    float m_springStrength = 150.0f;
    float m_damping = 20.0f;

    // Updated every frame

    DXSM::Vector3 m_localGrabOffset = DXSM::Vector3::Zero;

    DXSM::Vector3 m_targetPosition;
    // future work
    DXSM::Quaternion m_targetRotation;
};

class GrabSystem
{
public:
    GrabSystem();

    bool HasGrabPair(SE::UUID character);

    void Update(float deltaTime);

    void ProcessGrabInput(GameObject* gObj);
    void UpdateGrabTargets(float deltaTime);
    // void UpdateConstraints();
    void ProcessRelease();
    void ProcessThrow();

    void SetSystemContext(const SystemContext& context) { m_systemContext = context; }

    eastl::unordered_map<SE::UUID, GrabRuntime> m_grabPairs;
private:
    SystemContext m_systemContext;
};