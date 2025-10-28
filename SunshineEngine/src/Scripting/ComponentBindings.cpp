#include <Scripting/ComponentBindings.h>

#include <GameObject.h>
#include <Component/TransformComponent.h>
#include <Component/RenderComponent.h>
#include <Utils/DebugUtils.h>

namespace ScriptingBindings {

template<typename C>
inline C* GO_Get(GameObject* go) {
    return go->GetComponent<C>().get();
}

void RegisterAll(sol::state& lua) {
    // Math/value types
    lua.new_usertype<DXSM::Vector3>("Vector3",
        "x", &DXSM::Vector3::x,
        "y", &DXSM::Vector3::y,
        "z", &DXSM::Vector3::z
    );

    // Core components
    lua.new_usertype<TransformComponent>("TransformComponent",
#define ADD_FIELD(name) #name, &TransformComponent::name
        TRANSFORMCOMPONENT_LUA_FIELDS_APPLY(ADD_FIELD)
#undef ADD_FIELD
    );

    lua.new_usertype<RenderComponent>("RenderComponent",
#define ADD_METHOD(k, fn) k, fn
    RENDERCOMPONENT_LUA_METHODS_APPLY(ADD_METHOD)
#undef ADD_METHOD
    );

    // GameObject bridge (generic accessor helpers via macros)
    lua.new_usertype<GameObject>("GameObject",
#define MAKE_GO_GETTER_PAIR(T, NameStr) NameStr, &ScriptingBindings::GO_Get<T>
        GAMEOBJECT_LUA_GETTERS_APPLY(MAKE_GO_GETTER_PAIR)
#undef MAKE_GO_GETTER_PAIR
    );
}

}
