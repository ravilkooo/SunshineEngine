#include <Component/GrabComponent.h>

void GrabComponent::FromJson(const json& j)
{
    if (j.contains("m_holdDistance") && j["m_holdDistance"].is_number_float()) {
        m_holdDistance = j["m_holdDistance"].get<float>();
    }
    if (j.contains("m_maxGrabDistance") && j["m_maxGrabDistance"].is_number_float()) {
        m_maxGrabDistance = j["m_maxGrabDistance"].get<float>();
    }
    if (j.contains("m_throwImpulse") && j["m_throwImpulse"].is_number_float()) {
        m_throwImpulse = j["m_throwImpulse"].get<float>();
    }

    if (j.contains("m_grabSpringStrength") && j["m_grabSpringStrength"].is_number_float()) {
        m_grabSpringStrength = j["m_grabSpringStrength"].get<float>();
    }
    if (j.contains("m_grabSpringDamping") && j["m_grabSpringDamping"].is_number_float()) {
        m_grabSpringDamping = j["m_grabSpringDamping"].get<float>();
    }

    if (j.contains("m_rotateWithCamera") && j["m_rotateWithCamera"].is_boolean()) {
        m_rotateWithCamera = j["m_rotateWithCamera"].get<bool>();
    }
    if (j.contains("m_canGrabDynamicBodies") && j["m_canGrabDynamicBodies"].is_boolean()) {
        m_canGrabDynamicBodies = j["m_canGrabDynamicBodies"].get<bool>();
    }
    if (j.contains("m_canGrabKinematicBodies") && j["m_canGrabKinematicBodies"].is_boolean()) {
        m_canGrabKinematicBodies = j["m_canGrabKinematicBodies"].get<bool>();
    }
}

GrabComponent_Info::GrabComponent_Info()
{
    m_assignedComponent = eastl::make_unique<GrabComponent>();
}

json GrabComponent_Info::ToJson() const
{
    json j;
    if (m_assignedComponent) {
        j = nlohmann::json{
            {"m_holdDistance", m_assignedComponent->m_holdDistance},
            {"m_maxGrabDistance", m_assignedComponent->m_maxGrabDistance},
            {"m_throwImpulse", m_assignedComponent->m_throwImpulse},

            {"m_grabSpringStrength", m_assignedComponent->m_grabSpringStrength},
            {"m_grabSpringDamping", m_assignedComponent->m_grabSpringDamping},

            {"m_rotateWithCamera", m_assignedComponent->m_rotateWithCamera},
            {"m_canGrabDynamicBodies", m_assignedComponent->m_canGrabDynamicBodies},
            {"m_canGrabKinematicBodies", m_assignedComponent->m_canGrabKinematicBodies}
        };
    }
    return j;
}

void GrabComponent_Info::FromJson(const json& j)
{
    if (!m_assignedComponent)
        m_assignedComponent = eastl::make_unique<GrabComponent>();
    m_assignedComponent->FromJson(j);
}

