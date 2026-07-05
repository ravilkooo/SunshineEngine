#include "Component/MovingPlatformComponent.h"
#include "Component/TransformComponent.h"

#include <Scene.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

void RegisterMovingPlatformComponentLuaBindings()
{
}

#define MOV_PLAT_ADD_FIELD(name) #name, &MovingPlatformComponent::name
#define MOV_PLAT_FIELD_PAIRS MOVING_PLATFORM_COMPONENT_LUA_FIELDS_APPLY(MOV_PLAT_ADD_FIELD)

#define MOV_PLAT_METHOD_PAIRS

LUA_REGISTER_COMPONENT(MovingPlatformComponent, "MovingPlatformComponent", MOV_PLAT_FIELD_PAIRS, MOV_PLAT_METHOD_PAIRS, "getMovingPlatform")

#undef MOV_PLAT_ADD_FIELD
#undef MOV_PLAT_METHOD_PAIRS

