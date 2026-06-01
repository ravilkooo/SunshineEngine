#pragma once

#include <Windows/Keys.h>
#include <InputSystem/KeyInfo.h>
#include <InputSystem/PlayerInputSystem.h>

#include <EASTL/unordered_map.h>

class WorldEditor;
class CameraManager;

namespace SE_G {
	class MiniViewRenderer;
}

class PlayerSettingPanel
{
public:
	void OnImGuiRender(WorldEditor* worldEditor);

private:
	void DrawGameplayDetails(WorldEditor* worldEditor);
	void DrawMainCameraDetails(WorldEditor* worldEditor);

	void DrawKeyMappingEditor(PlayerInputSystem::KeyMapping_Info& mapping);
	static void DrawActionBindings(PlayerInputSystem::KeyMapping_Info& mapping);
	static void DrawAxisBindings(PlayerInputSystem::KeyMapping_Info& mapping);

	static bool DrawKeyCombo(
		const char* label,
		Keys& key, Keys& newKey);

	struct ActionConflictCache
	{
		eastl::unordered_map<Keys, uint32_t> KeyUsage;
	};

	static ActionConflictCache BuildActionConflictCache(const std::vector<PlayerInputSystem::ActionBinding>& bindings);

	static bool HasActionConflict(
		const ActionConflictCache& cache,
		Keys key);

	struct AxisConflictKey
	{
		Keys Key;
		std::string Name;

		bool operator==(const AxisConflictKey& rhs) const
		{
			return Key == rhs.Key &&
				Name == rhs.Name;
		}
	};

	struct AxisConflictKeyHasher
	{
		size_t operator()(const AxisConflictKey& value) const
		{
			size_t h1 = std::hash<int>()(
				static_cast<int>(value.Key));

			size_t h2 = std::hash<std::string>()(
				value.Name);

			return h1 ^ (h2 << 1);
		}
	};

	struct AxisConflictCache
	{
		eastl::unordered_map<
			AxisConflictKey,
			uint32_t,
			AxisConflictKeyHasher> Usage;
	};

	static AxisConflictCache BuildAxisConflictCache(const std::vector<PlayerInputSystem::AxisBinding>& bindings);

	static bool HasAxisConflict(
		const AxisConflictCache& cache,
		const PlayerInputSystem::AxisBinding& binding);
};
