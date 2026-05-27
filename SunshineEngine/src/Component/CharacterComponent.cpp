#include <Component/CharacterComponent.h>

CharacterComponent_Info::CharacterComponent_Info()
{
	m_assignedComponent = eastl::make_unique<CharacterComponent>();
}
