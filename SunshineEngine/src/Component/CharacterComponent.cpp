#include <Component/CharacterComponent.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

// Yaw Pitch
DXSM::Vector3 CharacterComponent::GetForwardDir() const
{
	return DXSM::Vector3(sin(m_yaw) * cos(m_pitch), sin(m_pitch), cos(m_yaw) * cos(m_pitch));
}

CharacterComponent_Info::CharacterComponent_Info()
{
	m_assignedComponent = eastl::make_unique<CharacterComponent>();
}

#define CHARC_ADD_PROPERTY(name, getter, setter) #name, sol::property(getter, setter)
#define CHARC_PROPERTY_PAIRS CHARACTERCOMPONENT_LUA_PROPERTIES_APPLY(CHARC_ADD_PROPERTY)

#define CHARC_METHOD_PAIRS

LUA_REGISTER_COMPONENT(
	CharacterComponent,
	"CharacterComponent",
	/* no fields */,
	CHARC_PROPERTY_PAIRS,
	CHARC_METHOD_PAIRS,
	"getCharacterComponent")

#undef CHARC_PROPERTY_PAIRS
#undef CHARC_METHOD_PAIRS
