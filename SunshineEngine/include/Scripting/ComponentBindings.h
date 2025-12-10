#pragma once

#include <sol/sol.hpp>
#include <GameObject/GameObject.h>

class TransformComponent;
class RenderComponent;
class PerceptionComponent;

namespace ScriptingBindings {
    // Registers all engine types and GameObject accessors to the given Lua state.
    void RegisterAll(sol::state& lua);

    // Generic helper: returns a raw component pointer for Lua from a GameObject
    template<typename C>
    inline C* GO_Get(GameObject* go) { return go->GetComponent<C>().get(); }
}

// Macro list of GameObject getters to expose in Lua
#ifndef GAMEOBJECT_LUA_GETTERS_APPLY
#define GAMEOBJECT_LUA_GETTERS_APPLY(F) \
    F(TransformComponent, "getTransform") , \
    F(RenderComponent, "getRender") , \
    F(PerceptionComponent, "getPerception")
#endif
