#include <PlayerObject/PlayerLuaKeyActionsMapping.h>
#include <PlayerObject/PlayerObject.h>
#include <Component/TransformComponent.h>
#include <Component/TriggerComponent.h>
#include <Component/PhysicsComponent.h>
#include <Component/CameraComponent.h>
#include <ParticleSystem/ParticleEmitterComponent.h>
#include <Graphics/Utils/Camera.h>

// #include <Physics/PhysicsSystem.h>
// #include <AI/Perception/PerceptionSystem.h>


#include <iostream>

PlayerLuaKeyActionsMapping::PlayerLuaKeyActionsMapping()
{
	m_keyActionMapping = eastl::unordered_map<Keys, eastl::string>();
}

PlayerLuaKeyActionsMapping::~PlayerLuaKeyActionsMapping()
{
}

bool PlayerLuaKeyActionsMapping::Initialize(const AssetPath& scriptPath)
{
	m_luaScriptPath = scriptPath;
	m_luaState = eastl::make_unique<sol::state>();

	// Open standard Lua libraries
	m_luaState->open_libraries(
		sol::lib::base,
		sol::lib::package,
		sol::lib::math,
		sol::lib::string,
		sol::lib::table,
		sol::lib::os
	);

	// Register C++ bindings
	RegisterLuaBindings();

	// Load the Lua script
	try {
		auto result = m_luaState->safe_script_file(WStringToUtf8(m_luaScriptPath.GetFullPath()).c_str());
		if (!result.valid()) {
			sol::error err = result;
			LogError("Failed to load Lua script: " + eastl::string(err.what()));
			return false;
		}
	}
	catch (const sol::error& e) {
		LogError("Lua script loading exception: " + eastl::string(e.what()));
		return false;
	}

	return true;
}

void PlayerLuaKeyActionsMapping::BindKey(Keys key, const eastl::string& luaFunctionName)
{
	m_keyActionMapping[key] = luaFunctionName;
}

void PlayerLuaKeyActionsMapping::BindKeyByString(const eastl::string& keyName, const eastl::string& luaFunctionName)
{
	// Simple string to Keys mapping (extend as needed)
	static const eastl::unordered_map<eastl::string, Keys> keyMap = {
		{"W", Keys::W}, {"A", Keys::A}, {"S", Keys::S}, {"D", Keys::D},
        {"E", Keys::E}, {"Q", Keys::Q},
		{"Space", Keys::Space},
        // {"Shift", Keys::ShiftKey},
		// {"Ctrl", Keys::ControlKey},
	};

	auto it = keyMap.find(keyName);
	if (it != keyMap.end()) {
		BindKey(it->second, luaFunctionName);
	} else {
		LogError("Unknown key name: " + keyName);
	}
}

void PlayerLuaKeyActionsMapping::InitKeyBindingFromJson(const json& j)
{
	// Load key-function mappings
	for (const auto& pairJson : j) {
		auto pair = KeyFunctionPair::FromJson(pairJson);
		BindKey(pair.key, pair.functionName);
	}
}

void PlayerLuaKeyActionsMapping::InitMouseActionHandler(const eastl::string& functionName)
{
	m_mouseActionsHandlingFunction = functionName;
}

void PlayerLuaKeyActionsMapping::UnbindKey(Keys key)
{
	m_keyActionMapping.erase(key);
}

bool PlayerLuaKeyActionsMapping::ExecuteMouseMoveAction(InputDevice::MouseMoveEventArgs mouseArgs)
{
	// If no Lua state or no handler name configured, abort
	if (!m_luaState) return false;
	if (m_mouseActionsHandlingFunction.empty()) return false;

	// try
	{
		sol::protected_function func = (*m_luaState)[m_mouseActionsHandlingFunction.c_str()];
		if (!func.valid()) {
			LogError("Lua mouse move handler not found: " + m_mouseActionsHandlingFunction);
			return false;
		}

		// Pass mouse delta (dx, dy) and wheel delta to Lua. Signature: function(dx, dy, wheelDelta)
		auto result = func(mouseArgs.Offset.x, mouseArgs.Offset.y, mouseArgs.WheelDelta);
		if (!result.valid()) {
			sol::error err = result;
			LogError("Lua execution error in mouse handler: " + eastl::string(err.what()));
			return false;
		}

		return true;
	}
	/*
	catch (const sol::error& e) {
		LogError("Lua exception in mouse handler: " + eastl::string(e.what()));
		return false;
	}
	*/
}

void PlayerLuaKeyActionsMapping::SetPlayerObject(PlayerObject* player)
{
	m_playerObject = player;

	// Update Lua global with player reference
	if (m_luaState && player) {
		(*m_luaState)["player"] = player;
	}
}

bool PlayerLuaKeyActionsMapping::IsKeyBound(Keys key) const
{
	return m_keyActionMapping.find(key) != m_keyActionMapping.end();
}

eastl::optional<eastl::string> PlayerLuaKeyActionsMapping::GetBoundFunction(Keys key) const
{
	auto it = m_keyActionMapping.find(key);
	if (it != m_keyActionMapping.end()) {
		return it->second;
	}
	return eastl::nullopt;
}

bool PlayerLuaKeyActionsMapping::ReloadScript()
{
	if (m_luaScriptPath.m_assetRelativePath.empty()) return false;
	return Initialize(m_luaScriptPath);
}

void PlayerLuaKeyActionsMapping::LogError(const eastl::string& message)
{
	// Replace with your engine's logging system
	std::cerr << "[PlayerLuaKeyActionsMapping Error] " << message.c_str() << std::endl;
}

void PlayerLuaKeyActionsMapping::RegisterLuaBindings()
{
	if (!m_luaState) return;

	m_luaState->new_usertype<SE::UUIDhilo>("UUID",
		sol::constructors<SE::UUIDhilo()>(),
		"hi", &SE::UUIDhilo::hi,
		"lo", &SE::UUIDhilo::lo,
		"toString", [](SE::UUIDhilo* self) { return SE::UUID(*self).ToString(); },
		"isEqual", [](SE::UUIDhilo* self, SE::UUIDhilo other) {
			return self->hi == other.hi && self->lo == other.lo;
		}
	);

	// Register Vector3 type
	m_luaState->new_usertype<DXSM::Vector3>("Vector3",
		sol::constructors<DXSM::Vector3(), DXSM::Vector3(float, float, float)>(),
		"x", &DXSM::Vector3::x,
		"y", &DXSM::Vector3::y,
		"z", &DXSM::Vector3::z,
		"Normalize",
		[](DXSM::Vector3* self) {
			return self->Normalize();
		}
		/*&DXSM::Vector3::Normalize*/,
		"Length", &DXSM::Vector3::Length
	);

	// Register Camera type
	m_luaState->new_usertype<SE_G::Camera>("Camera",
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
		/*
		// Update methods
		"Update", sol::overload(
			static_cast<void (SE_G::Camera::*)()>(&SE_G::Camera::Update),
			static_cast<void (SE_G::Camera::*)(const DXSM::Vector3)>(&SE_G::Camera::Update)
		)
		*/
	);

	// Register TransformComponent
	m_luaState->new_usertype<TransformComponent>("TransformComponent",
		sol::no_constructor,
		"m_position", &TransformComponent::m_position,
		"m_rotation", &TransformComponent::m_rotation,
		"m_scale", &TransformComponent::m_scaleFactor
	);

	// Register CameraComponent
	m_luaState->new_usertype<CameraComponent>("CameraComponent",
		sol::no_constructor,
		"getCamera", [](CameraComponent* self) { return self->GetCamera(); }
	);

	// Register PhysicsComponent
	m_luaState->new_usertype<PhysicsComponent>("PhysicsComponent",
		sol::no_constructor,
		"addForce", &PhysicsComponent::AddForce,
		"addImpulse", &PhysicsComponent::AddImpulse,
		"addTorque", &PhysicsComponent::AddTorque,
		"addAngularImpulse", &PhysicsComponent::AddAngularImpulse,
		"getAccumulatedForce", &PhysicsComponent::GetAccumulatedForce,
		"getAccumulatedTorque", &PhysicsComponent::GetAccumulatedTorque,
		"getAngularVelocity", &PhysicsComponent::GetAngularVelocity,
		"getLinearVelocity", &PhysicsComponent::GetLinearVelocity,
		"getPointVelocity", &PhysicsComponent::GetPointVelocity,
		"getPosition", &PhysicsComponent::GetPosition,
		"getRotation", &PhysicsComponent::GetRotation,
		"resetForce", &PhysicsComponent::ResetForce,
		"resetTorque", &PhysicsComponent::ResetTorque,
		"setAngularVelocity", &PhysicsComponent::SetAngularVelocity,
		"setLinearVelocity", &PhysicsComponent::SetLinearVelocity,

		"setPosition", [](PhysicsComponent* self, DXSM::Vector3 inVal) { return self->SetPosition(inVal); },
		"setRotation", [](PhysicsComponent* self, DXSM::Vector3 inVal) { return self->SetRotation(inVal); },
		"getGravityFactor", [](PhysicsComponent* self) { return self->GetGravityFactor(); },
		"setGravityFactor", [](PhysicsComponent* self, float inVal) { return self->SetGravityFactor(inVal); },
		"setActive", [](PhysicsComponent* self, bool active) { self->SetActive(active); },
		"isActive", [](PhysicsComponent* self) { return self->IsActive(); }
	);

	// Register PlayerObject
	m_luaState->new_usertype<PlayerObject>("PlayerObject",
		sol::no_constructor,
		"getTransform", [](PlayerObject* player) {
			return player->GetComponent<TransformComponent>().get();
		},
		"getPhysics", [](PlayerObject* player) {
			return player->GetComponent<PhysicsComponent>().get();
		},
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

	// Base GameObject type; component binders will append getters
	m_luaState->new_usertype<GameObject>("GameObject",
		sol::no_constructor,
		"getTransform", [](GameObject* player) {
			return player->GetComponent<TransformComponent>().get();
		},
		"getPhysics", [](GameObject* player) {
			return player->GetComponent<PhysicsComponent>().get();
		},
		"getParticleEmitter", [](GameObject* player) {
			return player->GetComponent<ParticleEmitterComponent>().get();
		},
		"getTrigger", [](GameObject* player) {
			return player->GetComponent<TriggerComponent>().get();
		},
		// "getPerception", [](GameObject* player) {
		// 	return player->GetComponent<PerceptionComponent>().get();
		// },
		// "getBehavior", [](GameObject* player) {
		// 	return player->GetComponent<BehaviorController>().get();
		// },
		"getName", [](GameObject* player) {
			return player->m_name.c_str();
		},
		"getUUID", [](GameObject* self) {
			return self->m_UUID.GetHilo();
		}
	);

	// Remove object from scene
	m_luaState->set_function("removeGameObjectByUUID", [](SE::UUIDhilo uuidhilo) {
		Scene::GetInstance().QueueGameObjectForDestruction(SE::UUID::FromHilo(uuidhilo));
		});

	// GetObject by UUID
	m_luaState->set_function("getGameObjectByUUID", [](SE::UUIDhilo uuidhilo) -> GameObject* {
		return Scene::GetInstance().GetGameObjectByUUID(SE::UUID::FromHilo(uuidhilo));
		});


	// m_luaState->set_function("getPerceptionSystem", []() -> PerceptionSystem& {
	// 	return PerceptionSystem::Get();
	// 	});


	// m_luaState->set_function("getGloabalGravity", []() -> DXSM::Vector3 {
	// 	return Scene::GetInstance().m_physicsSystem->GetGravity();
	// 	});
	// 
	// m_luaState->set_function("setGloabalGravity", [](DXSM::Vector3 inVal) {
	// 	return Scene::GetInstance().m_physicsSystem->SetGravity(inVal);
	// 	});

	// Helper functions
	(*m_luaState)["print"] = [](const std::string& msg) {
		// std::cout << "[Lua] " << msg << std::endl;
		printf("%s\n", msg.c_str());
	};
}