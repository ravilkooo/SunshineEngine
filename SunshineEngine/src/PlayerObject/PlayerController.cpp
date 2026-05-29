#include <PlayerObject/PlayerController.h>
#include <PlayerObject/PlayerObject.h>
#include <Graphics/Utils/Camera.h>
#include <Component/CameraComponent.h>

void PlayerController::SetPlayerObject(PlayerObject* player)
{
	m_player = player;
}

void PlayerController::HandleKeyDown(Keys key)
{
	// Feed input to InputManager for proper edge detection
	m_inputManager.ProcessKeyDown(key);

	// Update input state for this frame - computes edge events
	m_inputManager.Update();

	// Try Lua callback for key press (edge event)
	if (m_useLuaCallbacks && m_player) {
		// Only call Lua on the press event (not every frame while held)
		if (m_inputManager.IsKeyPressed(key))
		{
			m_player->m_luaActionMapping.ExecuteKeyAction(key, "pressed");
		}
		else if (m_inputManager.IsKeyDown(key))
		{
			m_player->m_luaActionMapping.ExecuteKeyAction(key, "down");
		}
	}
}

void PlayerController::HandleKeyUp(Keys key)
{
	// Feed input to InputManager for proper edge detection
	m_inputManager.ProcessKeyUp(key);

	// Update input state for this frame - computes edge events
	m_inputManager.Update();

	// Try Lua callback for key release (edge event)
	if (m_useLuaCallbacks && m_player) {
		// Only call Lua on the release event
		if (m_inputManager.IsKeyReleased(key)) {
			m_player->m_luaActionMapping.ExecuteKeyAction(key, "up");
		}
	}
}

void PlayerController::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	m_stickYawMoveDir = args.Offset.x * m_stickYawPitchSpeed;
	m_stickPitchMoveDir = -args.Offset.y * m_stickYawPitchSpeed;
		
	m_player->GetComponent<CameraComponent>()->GetCamera()->RotateSpringArmYawPitch(m_stickYawMoveDir, m_stickPitchMoveDir);

	m_player->m_luaActionMapping.ExecuteMouseMoveAction(args);
}

void PlayerController::ExecuteAllOnKeyDown()
{
	if (m_useLuaCallbacks && m_player) {
		for (auto key : m_inputManager.m_currentKeys)
		{
			m_player->m_luaActionMapping.ExecuteKeyAction(key, "down");
		}
	}
}

void PlayerController::UpdatePlayer(float deltaTime)
{
	/*
	auto pc = m_player->GetComponent<PhysicsComponent>();
	if (pc)
	{
		// Sync kinematic physics body with TransformComponent
		pc->AddForce(DXSM::Vector3(0, 0, 0.5f * deltaTime));
		pc->AddImpulse(DXSM::Vector3(0, 0.5f * deltaTime, 0));
	}
	*/

	// Update input state for this frame - computes edge events
	// m_inputManager.Update();

	ExecuteAllOnKeyDown();

	/*
	// Handle movement using InputManager (supports key held)
	if (m_inputManager.IsKeyDown(Keys::W) ||
		m_inputManager.IsKeyDown(Keys::A) ||
		m_inputManager.IsKeyDown(Keys::S) ||
		m_inputManager.IsKeyDown(Keys::D))
	{
		m_moveDir =
		{
			(m_inputManager.IsKeyDown(Keys::D) - m_inputManager.IsKeyDown(Keys::A)) * 1.0f,
			0.0f,
			(m_inputManager.IsKeyDown(Keys::W) - m_inputManager.IsKeyDown(Keys::S)) * 1.0f
		};
		m_moveDir.Normalize();
		m_moveDir = DXSM::Vector3::Transform(m_moveDir, m_player->m_playerCamera->rotateCamToForward);
		m_player->GetComponent<TransformComponent>()->m_position += m_moveDir * m_moveSpeed;
	}
	m_moveDir = DXSM::Vector3::Zero;
	*/

	m_stickYawMoveDir = 0.0f;
	m_stickPitchMoveDir = 0.0f;
}