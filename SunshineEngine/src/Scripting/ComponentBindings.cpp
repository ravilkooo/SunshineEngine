#include <Scripting/ComponentBindings.h>
#include <Scripting/AutoBindings.h>

#include <SimpleMath.h>
#include <Physics/PhysicsSystem.h>

#include <Graphics/Utils/Camera.h>
#include <GameObject/GameObject.h>
#include <PlayerObject/PlayerObject.h>
#include <Component/CameraComponent.h>
#include <Scene.h>
#include <Utils/UUID.h>
#include <Utils/DebugUtils.h>
#include <Utils/GameLogManager.h>

#include <AI/Perception/PerceptionSystem.h>
#include "AI/Behavior/MemoryBoard.h"
#include "AI/Behavior/BehaviorController.h"

#include "Audio/AudioSystem.h"

namespace DXSM = DirectX::SimpleMath;

namespace ScriptingBindings
{
    void RegisterAll(sol::state& lua)
    {
        // Register UUIDhilo struct for safe 32-bit access
        lua.new_usertype<SE::UUIDhilo>("UUID",
            sol::constructors<SE::UUIDhilo()>(),
            "hi", &SE::UUIDhilo::hi,
            "lo", &SE::UUIDhilo::lo,
            "toString", [](SE::UUIDhilo* self) { return SE::UUID::FromHilo(*self).ToString(); },
			"isEqual", [](SE::UUIDhilo* self, SE::UUIDhilo other) {
				return self->hi == other.hi && self->lo == other.lo;
			}
        );

        // Math/value types
        lua.new_usertype<DXSM::Vector3>("Vector3",
            sol::constructors<DXSM::Vector3(), DXSM::Vector3(float, float, float)>(),
            "x", &DXSM::Vector3::x,
            "y", &DXSM::Vector3::y,
            "z", &DXSM::Vector3::z,
			"normalize",
			[](DXSM::Vector3* self) {
				return self->Normalize();
			},
			"length", &DXSM::Vector3::Length
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
        lua.new_usertype<GameObject>("GameObject",
			sol::no_constructor,
			"getUUID", [](GameObject* self) { return self->m_UUID.GetHilo(); },
			"getName", [](GameObject* player) {
				return player->m_name.c_str();
			}
			);

		// Register PlayerObject
		lua.new_usertype<PlayerObject>("PlayerObject",
			sol::no_constructor,
			// "getTransform", [](PlayerObject* player) {
			// 	return player->GetComponent<TransformComponent>().get();
			// },
			// "getPhysics", [](PlayerObject* player) {
			// 	return player->GetComponent<PhysicsComponent>().get();
			// },
			"getCamera", [](PlayerObject* player) {
				return player->m_playerCamera.get();
			},
			"getCameraComponent", [](PlayerObject* player) {
				return player->GetComponent<CameraComponent>().get();
			},
			"getName", [](PlayerObject* player) {
				return player->m_name.c_str();
			},
			"getUUID", [](PlayerObject* self) {
				return self->m_UUID.GetHilo();
			}
		);

    	// Register audio
    	lua.set_function("getAudioSystem", []() -> AudioSystem& {
			return AudioSystem::Get();
		});

        // Execute all component binders registered via LUA_REGISTER_COMPONENT
        AutoBindings::RegisterAll(lua);
		
		// Remove object from scene
		lua.set_function("removeGameObjectByUUID", [](SE::UUIDhilo uuidhilo) {
			Scene::GetInstance().QueueGameObjectForDestruction(SE::UUID::FromHilo(uuidhilo));
			});

		// Get Object by UUID
		lua.set_function("getGameObjectByUUID", [](SE::UUIDhilo uuidhilo) -> GameObject* {
			return Scene::GetInstance().GetGameObjectByUUID(SE::UUID::FromHilo(uuidhilo));
			});

		// Get PlayerObject
		lua.set_function("getPlayerObject", []() -> PlayerObject* {
			return Scene::GetInstance().m_playerObject;
			});

        //lua.new_usertype<PerceptionSystem>("PerceptionSystem",
        //    sol::no_constructor, 
        //    "registerTeam", &PerceptionSystem::RegisterTeam
        //);

        lua.set_function("getPerceptionSystem", []() -> PerceptionSystem& {
            return PerceptionSystem::Get();
            });


		lua.set_function("getGlobalGravity", []() -> DXSM::Vector3 {
			return Scene::GetInstance().m_physicsSystem->GetGravity();
			});

		lua.set_function("setGlobalGravity", [](DXSM::Vector3 inVal) {
			return Scene::GetInstance().m_physicsSystem->SetGravity(inVal);
			});


		lua.set_function("LOG_GAME_INFO", [](const std::string& msg) {
			// std::cout << "[Lua] " << msg << std::endl;
			LOG_GAME_INFO("%s\n", msg.c_str());
			});
		
		/*
		lua.set_function("LOG_GAME_INFO", [](const std::string& fmt, sol::variadic_args args) {
			std::vector<sol::object> arg_objects(args.begin(), args.end());

			std::string message = std::string(fmt);  // fmt как базовая строка
			for (const auto& arg : arg_objects) {
				message += " " + arg.as<std::string>();
			}
			auto formatted = message;

			LOG_GAME_INFO("%s", formatted.c_str());  // Вызов оригинальной функции
		});
		*/
    }

} // namespace ScriptingBindings
