#include <Scripting/ComponentBindings.h>
#include <Scripting/AutoBindings.h>

#include <string>

void RegisterBouncePadComponentLuaBindings();
void RegisterMovingPlatformComponentLuaBindings();

#include <Physics/PhysicsSystem.h>

#include <Graphics/Utils/Camera.h>
#include <Graphics/Renderer/DeferredRenderer.h>

#include <GameObject/GameObject.h>

#include <Component/CameraComponent.h>

#include <CameraManager.h>

#include <Scene.h>
#include <Utils/UUID.h>
#include <Utils/DebugUtils.h>

#include <AI/Perception/PerceptionSystem.h>
#include "AI/Behavior/MemoryBoard.h"
#include "AI/Behavior/BehaviorController.h"

#include "Audio/AudioSystem.h"

#include <SimpleMath.h>
namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

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
			"length", &DXSM::Vector3::Length,
			sol::meta_function::addition, [](const DXSM::Vector3& a, const DXSM::Vector3& b) {
				return a + b;
			},
			sol::meta_function::subtraction, [](const DXSM::Vector3& a, const DXSM::Vector3& b) {
				return a - b;
			},
			sol::meta_function::multiplication, [](const DXSM::Vector3& a, const DXSM::Vector3& b) {
				return a * b;
			},
			sol::meta_function::multiplication, [](const DXSM::Vector3& a, float s) {
				return a * s;
			}
        );

		lua.new_usertype<DXSM::Vector2>("Vector2",
			sol::constructors<DXSM::Vector2(), DXSM::Vector2(float, float)>(),
			"x", &DXSM::Vector2::x,
			"y", &DXSM::Vector2::y,
			"normalize",
			[](DXSM::Vector2* self) {
				return self->Normalize();
			},
			"length", &DXSM::Vector2::Length,
			sol::meta_function::addition, [](const DXSM::Vector2& a, const DXSM::Vector2& b) {
				return a + b;
			},
			sol::meta_function::subtraction, [](const DXSM::Vector2& a, const DXSM::Vector2& b) {
				return a - b;
			},
			sol::meta_function::multiplication, [](const DXSM::Vector2& a, const DXSM::Vector2& b) {
				return a * b;
			},
			sol::meta_function::multiplication, [](const DXSM::Vector2& a, float s) {
				return a * s;
			}
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
			// Up/right/forward vector methods
			"setUp", &SE_G::Camera::SetUp,
			"getUp", &SE_G::Camera::GetUp,
			"getRight", &SE_G::Camera::GetRight,
			"getForward", &SE_G::Camera::GetForward,
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
			"rotateSpringArmYaw", &SE_G::Camera::RotateSpringArmYaw,
			"rotateSpringArmPitch", &SE_G::Camera::RotateSpringArmPitch,
			"rollSpringArm", &SE_G::Camera::RollSpringArm,
			"rotateSpringArmYawPitch", &SE_G::Camera::RotateSpringArmYawPitch,
			"rotateSpringArm", &SE_G::Camera::RotateSpringArm,
			// Camera Properties
			"getCameraRotation", &SE_G::Camera::GetCameraRotation,
			"setCameraRotation", &SE_G::Camera::SetCameraRotation,
			// SpringArm Properties
			"getSpringArmRotation", &SE_G::Camera::GetSpringArmRotation,
			"setSpringArmRotation", &SE_G::Camera::SetSpringArmRotation,
			"getSpringArmRootOffset", &SE_G::Camera::GetSpringArmRootOffset,
			"setSpringArmRootOffset", &SE_G::Camera::SetSpringArmRootOffset,
			"getSpringArmLength", &SE_G::Camera::GetSpringArmLength,
			"setSpringArmLength", &SE_G::Camera::SetSpringArmLength,
			// Camera mode
			"switchToFPSMode", &SE_G::Camera::SwitchToFPSMode,

			"setAsMainCamera", [](SE_G::Camera* camera) {
				Scene::GetInstance().m_mainCameraUUID = camera->GetAssignedUUID();
				Scene::GetInstance().m_renderer->SetMainCamera(
					Scene::GetInstance().m_cameraManager->GetCameraByUUID(camera->GetAssignedUUID()));
				return;
			}

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

    	// Register audio
    	lua.set_function("getAudioSystem", []() -> AudioSystem& {
			return AudioSystem::Get();
		});

        // Execute all component binders registered via LUA_REGISTER_COMPONENT
		RegisterBouncePadComponentLuaBindings();
		RegisterMovingPlatformComponentLuaBindings();
        AutoBindings::RegisterAll(lua);
		
		// Remove object from scene
		lua.set_function("removeGameObjectByUUID", [](SE::UUIDhilo uuidhilo) {
			Scene::GetInstance().QueueGameObjectForDestruction(SE::UUID::FromHilo(uuidhilo));
			});

		// Get Object by UUID
		lua.set_function("getGameObjectByUUID", [](SE::UUIDhilo uuidhilo) -> GameObject* {
			return Scene::GetInstance().GetGameObjectByUUID(SE::UUID::FromHilo(uuidhilo));
			});

		// Get MainCamera
		lua.set_function("getMainCamera", []() {
			return Scene::GetInstance().m_cameraManager->GetCameraByUUID(Scene::GetInstance().m_mainCameraUUID).get();
			});

		// Get Camera by UUID
		lua.set_function("getCameraByUUID", [](SE::UUIDhilo uuidhilo) {
			return Scene::GetInstance().m_cameraManager->GetCameraByUUID(SE::UUID::FromHilo(uuidhilo)).get();
			});

		// Get Camera by UUID
		lua.set_function("setCameraByUUID", [](SE::UUIDhilo uuidhilo) {
			auto camUUID = SE::UUID::FromHilo(uuidhilo);
			if (Scene::GetInstance().m_cameraManager->HasCameraByUUID(camUUID)) {
				Scene::GetInstance().m_mainCameraUUID = camUUID;

				Scene::GetInstance().m_renderer->SetMainCamera(
					Scene::GetInstance().m_cameraManager->GetCameraByUUID(camUUID));
			}
			return;
			});

        lua.set_function("getPerceptionSystem", []() -> PerceptionSystem& {
            return PerceptionSystem::Get();
            });


		lua.set_function("getGlobalGravity", []() -> DXSM::Vector3 {
			return Scene::GetInstance().m_physicsSystem->GetGravity();
			});

		lua.set_function("setGlobalGravity", [](DXSM::Vector3 inVal) {
			return Scene::GetInstance().m_physicsSystem->SetGravity(inVal);
			});

		// PlayerInputSystem
		lua.new_usertype<PlayerInputSystem>("InputSystem",
			sol::no_constructor,
			"isPressed", [](PlayerInputSystem* self, std::string action) {
				return self->IsPressed(action);
			},
			"isReleased", [](PlayerInputSystem* self, std::string action) { return self->IsReleased(action); },
			"isHeld", [](PlayerInputSystem* self, std::string action) { return self->IsHeld(action); },
			"getAxis", [](PlayerInputSystem* self, std::string axisName) { return self->GetAxis(axisName); },
			"getAxis2D", [](PlayerInputSystem* self, std::string axisHorizontal, std::string axisVertical)
			{ return self->GetAxis2D(axisHorizontal, axisVertical); },
			"getMouseDeltaX", [](PlayerInputSystem* self) { return self->GetMouseDeltaX(); },
			"getMouseDeltaY", [](PlayerInputSystem* self) { return self->GetMouseDeltaY(); },
			"getMouseX", [](PlayerInputSystem* self) { return self->GetMouseX(); },
			"getMouseY", [](PlayerInputSystem* self) { return self->GetMouseY(); },
			"getMouseWheelDelta", [](PlayerInputSystem* self) { return self->GetMouseWheelDelta(); }
		);

		lua.set_function("getInputSystem", []() {
			return PlayerInputSystem::GetInstance();
			});
    }

} // namespace ScriptingBindings
