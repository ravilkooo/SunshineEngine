#include "Component/BouncePadComponent.h"
#include "Component/CharacterComponent.h"

#include <Scene.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

void BouncePadComponent::BounceCharacter(SE::UUID characterUUID)
{
    auto character = Scene::GetInstance().GetGameObjectByUUID(characterUUID)->GetComponent<CharacterComponent>();
    if (character)
    {
        character->m_bounced = true;
        character->m_bounceSpeed = m_minBounceVelocity;
    }
}

#define BPC_ADD_FIELD(name) #name, &BouncePadComponent::name
#define BPC_FIELD_PAIRS BOUNCEPADCOMPONENT_LUA_FIELDS_APPLY(BPC_ADD_FIELD)

#define BPC_ADD_METHOD_WITH_LEAD(k, fn) , k, fn
#define BPC_METHOD_PAIRS BOUNCEPADCOMPONENT_LUA_METHODS_APPLY(BPC_ADD_METHOD_WITH_LEAD)

LUA_REGISTER_COMPONENT(BouncePadComponent, "BouncePadComponent", BPC_FIELD_PAIRS, BPC_METHOD_PAIRS, "getBouncePad")
#undef BPC_ADD_FIELD
#undef  BPC_ADD_METHOD_WITH_LEAD

void RegisterBouncePadComponentLuaBindings()
{
}
