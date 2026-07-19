#pragma once

#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/shared_ptr.h>

#include <Utils/UUID.h>
#include <SystemContext.h>

#include <SimpleMath.h>

namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

class GrabComponent;
class CharacterComponent;
class GameObject;

struct GrabInput
{
    GameObject* gameObject;
    SE::UUID character;

    enum GrabAction { Grab, Release, Throw } action;
};

class GrabRuntime
{
public:

    SE::UUID m_character;
    SE::UUID m_grabbedObject;

    bool m_isDynamic = true;

    // Updated every frame

    DXSM::Vector3 m_localGrabOffset = DXSM::Vector3::Zero;

    DXSM::Vector3 m_targetPosition;
    // future work
    bool m_keepOriginalRotation = true;
    DXSM::Quaternion m_targetRotation;
};

class GrabSystem
{
public:
    GrabSystem();

    bool HasGrabPair(SE::UUID character);

    void EnqueueGrabInput(GrabInput grabInput);
    void FlushGrabQueue();

    void Update(float deltaTime);
    void UpdateGrabTargets(float deltaTime);

    void ProcessGrab(GameObject* gObj);
    void ProcessRelease(GameObject* gameObj);
    void ProcessThrow(GameObject* gameObj);

    void SetSystemContext(const SystemContext& context) { m_systemContext = context; }

    eastl::unordered_map<SE::UUID, GrabRuntime> m_grabPairs;
private:
    SystemContext m_systemContext;

    eastl::vector<GrabInput> m_grabInputQueue;
};