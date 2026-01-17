#include <Scripting/ComponentBindings.h>

#include <GameObject/GameObject.h>
#include <Graphics/Utils/Camera.h>
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
            sol::constructors<DXSM::Vector3(), DXSM::Vector3(float, float, float)>(),
            "x", &DXSM::Vector3::x,
            "y", &DXSM::Vector3::y,
            "z", &DXSM::Vector3::z
        );

		// Register Camera type
		lua.new_usertype<SE_G::Camera>("Camera",
			sol::no_constructor,
			// Properties (read-only)
			"forward", sol::readonly(&SE_G::Camera::forward),
			"up", sol::readonly(&SE_G::Camera::up),
			"right", sol::readonly(&SE_G::Camera::right),
			"position", sol::readonly(&SE_G::Camera::position),
			// delta time
			"deltaTime", sol::readonly(&SE_G::Camera::m_deltaTime),
			// Position methods
			"setPosition", &SE_G::Camera::SetPosition,
			"getPosition", &SE_G::Camera::GetPosition,
			// Target methods
			"setTarget", &SE_G::Camera::SetTarget,
			"getTarget", &SE_G::Camera::GetTarget,
			// Up vector methods
			"setUp", &SE_G::Camera::SetUp,
			"getUp", &SE_G::Camera::GetUp,
			// Near/Far Z methods
			"setNearZ", &SE_G::Camera::SetNearZ,
			"getNearZ", &SE_G::Camera::GetNearZ,
			"setFarZ", &SE_G::Camera::SetFarZ,
			"getFarZ", &SE_G::Camera::GetFarZ,
			// Reference length
			"setReferenceLen", &SE_G::Camera::SetReferenceLen,
			"getReferenceLen", &SE_G::Camera::GetReferenceLen,
			// View dimensions
			"getViewWidth", &SE_G::Camera::GetViewWidth,
			"getViewHeight", &SE_G::Camera::GetViewHeight,
			// Movement methods
			"moveForward", &SE_G::Camera::MoveForward,
			"moveBackward", &SE_G::Camera::MoveBackward,
			"moveLeft", &SE_G::Camera::MoveLeft,
			"moveRight", &SE_G::Camera::MoveRight,
			"moveUp", &SE_G::Camera::MoveUp,
			"moveDown", &SE_G::Camera::MoveDown,
			// Rotation methods
			"rotateYaw", &SE_G::Camera::RotateYaw,
			"rotatePitch", &SE_G::Camera::RotatePitch,
			// Stick Properties
			"getStickDirection", &SE_G::Camera::GetStickDirection,
			"getStickLength", &SE_G::Camera::GetStickLength,
			"setStickLength", &SE_G::Camera::SetStickLength,
			// Camera mode
			"switchToFPSMode", &SE_G::Camera::SwitchToFPSMode

			// // Update methods
			// "Update", sol::overload(
			// 	static_cast<void (SE_G::Camera::*)()>(&SE_G::Camera::Update),
			// 	static_cast<void (SE_G::Camera::*)(const DXSM::Vector3)>(&SE_G::Camera::Update)
			// )
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
