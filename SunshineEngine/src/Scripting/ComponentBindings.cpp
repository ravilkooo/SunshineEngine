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
			sol::constructors<
				DXSM::Vector3(),
				DXSM::Vector3(float, float, float),
				DXSM::Vector3(const DXSM::Vector3&)>(),
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
			sol::constructors<DXSM::Vector2(), DXSM::Vector2(float, float), DXSM::Vector2(const DXSM::Vector2&)>(),
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
			// Properties
			"up", sol::property(&SE_G::Camera::GetUp, &SE_G::Camera::SetUp),
			"position", sol::property(&SE_G::Camera::GetPosition, &SE_G::Camera::SetPosition),
			"target", sol::property(&SE_G::Camera::GetTarget, &SE_G::Camera::SetTarget),
			"nearZ", sol::property(&SE_G::Camera::GetNearZ, &SE_G::Camera::SetNearZ),
			"farZ", sol::property(&SE_G::Camera::GetFarZ, &SE_G::Camera::SetFarZ),
			"referenceLen", sol::property(&SE_G::Camera::GetReferenceLen, &SE_G::Camera::SetReferenceLen),
			"cameraRotation", sol::property(&SE_G::Camera::GetCameraRotation, &SE_G::Camera::SetCameraRotation),
			"springArmRotation", sol::property(&SE_G::Camera::GetSpringArmRotation, &SE_G::Camera::SetSpringArmRotation),
			"springArmRootOffset", sol::property(&SE_G::Camera::GetSpringArmRootOffset, &SE_G::Camera::SetSpringArmRootOffset),
			"springArmLength", sol::property(&SE_G::Camera::GetSpringArmLength, &SE_G::Camera::SetSpringArmLength),
			// Vector methods
			"getRight", &SE_G::Camera::GetRight,
			"getForward", &SE_G::Camera::GetForward,
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
			// SpringArm Properties
			"zoomSpringArm", &SE_G::Camera::ZoomSpringArm,
			// Camera mode
			"switchToFPSMode", &SE_G::Camera::SwitchToFPSMode,
			//
			"setAsMainCamera", [](SE_G::Camera* camera) {
				Scene::GetInstance().m_mainCameraUUID = camera->GetAssignedUUID();
				Scene::GetInstance().m_renderer->SetMainCamera(
					Scene::GetInstance().m_cameraManager->GetCameraByUUID(camera->GetAssignedUUID()));
				return;
			}
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
			"isPressed", &PlayerInputSystem::IsPressed,
			"isReleased", &PlayerInputSystem::IsReleased,
			"isHeld", &PlayerInputSystem::IsHeld,
			"getAxis", &PlayerInputSystem::GetAxis,
			"getAxis2D", &PlayerInputSystem::GetAxis2D,
			"getMouseDeltaX", &PlayerInputSystem::GetMouseDeltaX,
			"getMouseDeltaY", &PlayerInputSystem::GetMouseDeltaY,
			"getMouseX", &PlayerInputSystem::GetMouseX,
			"getMouseY", &PlayerInputSystem::GetMouseY,
			"getMouseWheelDelta", &PlayerInputSystem::GetMouseWheelDelta
		);

		lua.set_function("getInputSystem", []() {
			return PlayerInputSystem::GetInstance();
			});
    }

} // namespace ScriptingBindings
