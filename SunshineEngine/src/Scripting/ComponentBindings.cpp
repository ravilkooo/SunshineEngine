#include <Scripting/ComponentBindings.h>

#include <GameObject/GameObject.h>
#include <Scripting/AutoBindings.h>
#include <Utils/DebugUtils.h>
#include <SimpleMath.h>
#include <AI/Perception/PerceptionSystem.h>
#include "AI/Behavior/MemoryBoard.h"
#include "AI/Behavior/BehaviorController.h"

namespace DXSM = DirectX::SimpleMath;

namespace ScriptingBindings
{
    void RegisterAll(sol::state& lua)
    {
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

        //lua.new_usertype<PerceptionSystem>("PerceptionSystem",
        //    sol::no_constructor, 
        //    "registerTeam", &PerceptionSystem::RegisterTeam
        //);

        lua.set_function("getPerceptionSystem", []() -> PerceptionSystem& {
            return PerceptionSystem::Get();
            });

        lua.set_function("createMemoryBoard", []() {
            return std::make_unique<MemoryBoard>();
            });

        lua.set_function("createAction", [](const std::string& name) {
            return std::make_unique<Action>(name);
            });

        lua.set_function("createPattern", []() {
            return std::make_unique<Pattern>();
            });

        lua.set_function("createState", []() {
            return std::make_unique<State>();
            });
    }

} // namespace ScriptingBindings
