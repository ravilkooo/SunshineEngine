#include <PlayerObject/PlayerController.h>
#include <PlayerObject/PlayerObject.h>

void PlayerController::SetPlayerObject(PlayerObject* player)
{
	m_player = player;
}

void PlayerController::HandleKeyDown(Keys key)
{
	// InputDevice::instance->IsKeyDown(key);
	/*
	m_moveDir = DXSM::Vector3::Zero;
	auto forward = m_player->m_playerCamera->forward;
	auto up = m_player->m_playerCamera->up;
	DXSM::Vector3 right = forward.Cross(up);
	*/
	switch (key)
	{
	case Keys::W:
	case Keys::S:
	case Keys::D:
	case Keys::A:
	{
		m_isKeyPressed[key] = true;
		break;
	}
	}
	/*
	m_moveDir.Normalize();
	m_moveDir = DXSM::Vector3::Transform(m_moveDir, m_player->m_playerCamera->rotateCamToForward);
	*/
}

void PlayerController::HandleKeyUp(Keys key)
{
	switch (key)
	{
	case Keys::W:
	case Keys::S:
	case Keys::D:
	case Keys::A:
	{
		m_isKeyPressed[key] = false;
		break;
	}
	}
	/*
	m_moveDir.Normalize();
	m_moveDir = DXSM::Vector3::Transform(m_moveDir, m_player->m_playerCamera->rotateCamToForward);
	*/
}

void PlayerController::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	m_stickYawMoveDir = args.Offset.x * m_stickYawPitchSpeed;
	m_stickPitchMoveDir = args.Offset.y * m_stickYawPitchSpeed;
}

void PlayerController::UpdatePlayer(float deltaTime)
{
	m_player->m_playerCamera->RotateStickYawPitch(deltaTime * m_stickYawMoveDir, deltaTime * m_stickPitchMoveDir);

	if (m_isKeyPressed[Keys::W] ||
		m_isKeyPressed[Keys::A] ||
		m_isKeyPressed[Keys::S] ||
		m_isKeyPressed[Keys::D])
	{
		m_moveDir =
		{
			(m_isKeyPressed[Keys::D] - m_isKeyPressed[Keys::A]) * 1.0f,
			0.0f,
			(m_isKeyPressed[Keys::W] - m_isKeyPressed[Keys::S]) * 1.0f
		};
		m_moveDir.Normalize();
		m_moveDir = DXSM::Vector3::Transform(m_moveDir, m_player->m_playerCamera->rotateCamToForward);
		m_player->GetComponent<TransformComponent>()->m_position += m_moveDir * m_moveSpeed;
	}

	m_stickYawMoveDir = 0.0f;
	m_stickPitchMoveDir = 0.0f;
	m_moveDir = DXSM::Vector3::Zero;
}