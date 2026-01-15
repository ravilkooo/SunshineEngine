#pragma once

class PlayerObject_Info;

class PlayerSettingPanel
{
public:
	PlayerObject_Info* m_playerObject = nullptr;

	void OnImGuiRender();
	void SetPlayerObject(PlayerObject_Info* playerObj);

private:
	void DrawPlayerObjectDetails();
	void DrawPlayerCameraDetails();
	void DrawPlayerControllerDetails();
};
