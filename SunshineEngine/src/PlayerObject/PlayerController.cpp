#include <PlayerObject/PlayerController.h>
#include <PlayerObject/PlayerObject.h>

void PlayerController::SetPlayerObject(PlayerObject* player)
{
	m_player = player;
}

void PlayerController::HandleKeyDown(Keys key)
{
	/*
	// Try Lua callback first if enabled
	if (m_useLuaCallbacks && m_player) {
		bool handled = m_player->m_luaActionMapping.ExecuteKeyAction(key, "down");
		if (handled) {
			// Lua handled the input, update key state
			m_isKeyPressed[key] = true;
			return;
		}
	}
	return;
	*/

	// Feed input to InputManager for proper edge detection
	m_inputManager.ProcessKeyDown(key);

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
	/*
	// Try Lua callback first if enabled
	if (m_useLuaCallbacks && m_player) {
		bool handled = m_player->m_luaActionMapping.ExecuteKeyAction(key, "up");
		if (handled) {
			// Lua handled the input, update key state
			m_isKeyPressed[key] = false;
			return;
		}
	}
	return;
	*/

	// Feed input to InputManager for proper edge detection
	m_inputManager.ProcessKeyUp(key);

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
	m_stickPitchMoveDir = args.Offset.y * m_stickYawPitchSpeed;
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
	// Update input state for this frame - computes edge events
	m_inputManager.Update();

	// Handle camera rotation
	m_player->m_playerCamera->RotateStickYawPitch(deltaTime * m_stickYawMoveDir, deltaTime * m_stickPitchMoveDir);
	ExecuteAllOnKeyDown();

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

	m_stickYawMoveDir = 0.0f;
	m_stickPitchMoveDir = 0.0f;
	m_moveDir = DXSM::Vector3::Zero;
}