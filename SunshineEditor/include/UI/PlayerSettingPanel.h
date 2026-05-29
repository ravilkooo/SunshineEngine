#pragma once

class PlayerObject_Info;
class WorldEditor;
class CameraManager;

namespace SE_G {
	class MiniViewRenderer;
}

class PlayerSettingPanel
{
public:
	PlayerObject_Info* m_playerObject = nullptr;

	void OnImGuiRender(WorldEditor* worldEditor);
	void SetPlayerObject(PlayerObject_Info* playerObj);

private:
	void DrawGameplayDetails(WorldEditor* worldEditor);
	void DrawMainCameraDetails(WorldEditor* worldEditor);
	void DrawControllerDetails();
};
