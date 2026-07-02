#pragma once

#include <sol/sol.hpp>
#include <GameObject/GameObject.h>

class TransformComponent;
class RenderComponent;
class PhysicsComponent;
class TriggerComponent;
class PerceptionComponent;
class BehaviorController;
class ParticleEmitterComponent;
class CameraComponent;
class CharacterComponent;
class CharacterControllerComponent;
class BouncePadComponent;
class MovingPlatformComponent;

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
    F(PhysicsComponent, "getPhysics") , \
    F(TriggerComponent, "getTrigger") , \
    F(BehaviorController, "getBehavior") , \
    F(PerceptionComponent, "getPerception"), \
    F(BouncePadComponent, "getBouncePad"), \
    F(ParticleEmitterComponent, "getParticleEmitter"), \
    F(CameraComponent, "getCameraComponent"), \
    F(CharacterComponent, "getCharacterComponent"), \
    F(CharacterControllerComponent, "getCharacterController") \
    F(MovingPlatformComponent, "getMovingPlatform")
#endif
