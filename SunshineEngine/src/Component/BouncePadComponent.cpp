#include "Component/BouncePadComponent.h"
#include "Component/CharacterComponent.h"
#include "Component/TransformComponent.h"

#include <Scene.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

BouncePadComponent::BouncePadComponent(TransformComponent* tc)
{
    m_assignedTransform = tc;
}

void BouncePadComponent::BounceCharacter(SE::UUID characterUUID)
{
    auto character = Scene::GetInstance().GetGameObjectByUUID(characterUUID)->GetComponent<CharacterComponent>();
    BounceCharacter(character.get());
}

void BouncePadComponent::BounceCharacter(CharacterComponent* character)
{
    if (character)
    {
        character->m_bounced = true;
        character->m_bounceSpeed = m_minBounceVelocity;

        if (m_assignedTransform)
        {
            DXSM::Matrix lwMat = m_assignedTransform->GetLocalTransformMatrix()
                * m_assignedTransform->GetWorldMatrix();
            
            lwMat._41 = 0;
            lwMat._42 = 0;
            lwMat._43 = 0;
            lwMat._44 = 1;

            const auto lwMatInvTranspose = (lwMat.Invert()).Transpose();
            character->m_bounceNormal = DXSM::Vector3::Transform(DXSM::Vector3(0, 1, 0), lwMatInvTranspose);
        }
    }
}

void RegisterBouncePadComponentLuaBindings()
{
}

#define BPC_ADD_PROPERTY(name, getter, setter) #name, sol::property(getter, setter)
#define BPC_PROPERTY_PAIRS BOUNCEPADCOMPONENT_LUA_PROPERTIES_APPLY(BPC_ADD_PROPERTY)

#define BPC_ADD_METHOD_WITH_LEAD(k, fn) , k, fn
#define BPC_METHOD_PAIRS BOUNCEPADCOMPONENT_LUA_METHODS_APPLY(BPC_ADD_METHOD_WITH_LEAD)

LUA_REGISTER_COMPONENT(
    BouncePadComponent,
    "BouncePadComponent",
    /* no fields */,
    BPC_PROPERTY_PAIRS,
    BPC_METHOD_PAIRS,
    "getBouncePad")
    
#undef BPC_ADD_PROPERTY
#undef  BPC_ADD_METHOD_WITH_LEAD
