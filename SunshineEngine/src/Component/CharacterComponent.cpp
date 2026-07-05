#include <Component/CharacterComponent.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

CharacterComponent_Info::CharacterComponent_Info()
{
	m_assignedComponent = eastl::make_unique<CharacterComponent>();
}

#define CHARC_ADD_FIELD(name) #name, &CharacterComponent::name
#define CHARC_FIELD_PAIRS CHARACTERCOMPONENT_LUA_FIELDS_APPLY(CHARC_ADD_FIELD)

#define CHARC_METHOD_PAIRS

LUA_REGISTER_COMPONENT(
	CharacterComponent,
	"CharacterComponent",
	CHARC_FIELD_PAIRS,
	/* no properties */,
	CHARC_METHOD_PAIRS,
	"getCharacterComponent")

#undef CHARC_ADD_FIELD
#undef CHARC_METHOD_PAIRS
