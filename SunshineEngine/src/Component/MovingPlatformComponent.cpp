#include "Component/MovingPlatformComponent.h"
#include "Component/TransformComponent.h"

#include <Scene.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

void RegisterMovingPlatformComponentLuaBindings()
{
}

#define MOV_PLAT_ADD_PROPERTY(name, getter, setter) #name, sol::property(getter, setter)
#define MOV_PLAT_PROPERTY_PAIRS MOVING_PLATFORM_COMPONENT_LUA_PROPERTIES_APPLY(MOV_PLAT_ADD_PROPERTY)

#define MOV_PLAT_METHOD_PAIRS

LUA_REGISTER_COMPONENT(
	MovingPlatformComponent,
	"MovingPlatformComponent",
	/* no fields */,
	MOV_PLAT_PROPERTY_PAIRS,
	MOV_PLAT_METHOD_PAIRS,
	"getMovingPlatform")

#undef MOV_PLAT_PROPERTY_PAIRS
#undef MOV_PLAT_METHOD_PAIRS
