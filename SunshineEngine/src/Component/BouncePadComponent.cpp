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
            const auto wMat = m_assignedTransform->GetWorldMatrix();
            DXSM::Matrix A = wMat;
            // Correct ?
            A._41 = 0;
            A._42 = 0;
            A._43 = 0;
            A._44 = 1;

            const auto wMatInvTranspose = (A.Invert()).Transpose();
            character->m_bounceNormal = DXSM::Vector3::Transform(DXSM::Vector3(0, 1, 0), wMatInvTranspose);
        }
    }
}

void RegisterBouncePadComponentLuaBindings()
{
}

#define BPC_ADD_FIELD(name) #name, &BouncePadComponent::name
#define BPC_FIELD_PAIRS BOUNCEPADCOMPONENT_LUA_FIELDS_APPLY(BPC_ADD_FIELD)

#define BPC_ADD_METHOD_WITH_LEAD(k, fn) , k, fn
#define BPC_METHOD_PAIRS BOUNCEPADCOMPONENT_LUA_METHODS_APPLY(BPC_ADD_METHOD_WITH_LEAD)

LUA_REGISTER_COMPONENT(
    BouncePadComponent,
    "BouncePadComponent",
    BPC_FIELD_PAIRS,
    /* no properties */,
    BPC_METHOD_PAIRS,
    "getBouncePad")
#undef BPC_ADD_FIELD
#undef  BPC_ADD_METHOD_WITH_LEAD
