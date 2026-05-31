#pragma once

#include <Windows/Keys.h>
#include <InputSystem/KeyInfo.h>
#include <InputSystem/PlayerInputSystem.h>

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

	void DrawKeyMappingEditor(PlayerInputSystem::KeyMapping_Info& mapping);
	static void DrawActionBindings(PlayerInputSystem::KeyMapping_Info& mapping);
	static void DrawAxisBindings(PlayerInputSystem::KeyMapping_Info& mapping);

	static bool DrawKeyCombo(
		const char* label,
		Keys& key, Keys& newKey);
};
