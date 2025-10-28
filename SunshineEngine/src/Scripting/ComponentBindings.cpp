#include <Scripting/ComponentBindings.h>

#include <GameObject.h>
#include <Scripting/AutoBindings.h>
#include <Utils/DebugUtils.h>
#include <SimpleMath.h>
namespace DXSM = DirectX::SimpleMath;

namespace ScriptingBindings {

void RegisterAll(sol::state& lua) {
    // Math/value types
    lua.new_usertype<DXSM::Vector3>("Vector3",
        "x", &DXSM::Vector3::x,
        "y", &DXSM::Vector3::y,
        "z", &DXSM::Vector3::z
    );

    // Base GameObject type; component binders will append getters
    lua.new_usertype<GameObject>("GameObject");

    // Execute all component binders registered via LUA_REGISTER_COMPONENT
    AutoBindings::RegisterAll(lua);
}

} // namespace ScriptingBindings
