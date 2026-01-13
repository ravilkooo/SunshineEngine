#pragma once

#include "sol/sol.hpp"
#include <EASTL/unordered_map.h>
#include <EASTL/string.h>
#include <EASTL/optional.h>
#include <EASTL/unique_ptr.h>

#include <Utils/AssetPath.h>

#include <Windows/Keys.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class PlayerObject;

class PlayerLuaKeyActionsMapping
{
public:
	PlayerLuaKeyActionsMapping();
	~PlayerLuaKeyActionsMapping();

	// Initialize Lua state and load script
	bool Initialize(const AssetPath& scriptPath);

	// Bind a key to a Lua function name
	void BindKey(Keys key, const eastl::string& luaFunctionName);
	void BindKeyByString(const eastl::string& keyName, const eastl::string& luaFunctionName);
	void InitBindingFromJson(const json& j);

	// Remove a key binding
	void UnbindKey(Keys key);

	// Execute Lua function for a specific key
	template<typename... Args>
	bool ExecuteKeyAction(Keys key, Args&&... args)
	{
		if (!m_luaState) return false;

		auto it = m_keyActionMapping.find(key);
		if (it == m_keyActionMapping.end()) return false;

		return ExecuteLuaFunction(it->second, eastl::forward<Args>(args)...);
	}

	// Execute a Lua function by name with error handling
	template<typename... Args>
	bool ExecuteLuaFunction(const eastl::string& functionName, Args&&... args)
	{
		if (!m_luaState) return false;

		try {
			sol::protected_function func = (*m_luaState)[functionName.c_str()];
			if (!func.valid()) {
				LogError("Lua function not found: " + functionName);
				return false;
			}

			auto result = func(eastl::forward<Args>(args)...);
			if (!result.valid()) {
				sol::error err = result;
				LogError("Lua execution error in " + functionName + ": " + eastl::string(err.what()));
				return false;
			}

			return true;
		}
		catch (const sol::error& e) {
			LogError("Lua exception: " + eastl::string(e.what()));
			return false;
		}
	}

	// Get the Lua state (for advanced usage)
	sol::state* GetLuaState() { return m_luaState.get(); }

	// Set player object reference for Lua callbacks
	void SetPlayerObject(PlayerObject* player);

	// Check if a key is bound
	bool IsKeyBound(Keys key) const;

	// Get bound function name for a key
	eastl::optional<eastl::string> GetBoundFunction(Keys key) const;

	// Reload the Lua script
	bool ReloadScript();

private:
	// Lua script configuration
	AssetPath m_luaScriptPath;

	eastl::unique_ptr<sol::state> m_luaState;

	// Key -> Lua function name mapping
	eastl::unordered_map<Keys, eastl::string> m_keyActionMapping;

	// Player object reference (passed to Lua)
	PlayerObject* m_playerObject = nullptr;

	// Helper function to log errors
	void LogError(const eastl::string& message);

	// Register C++ objects and functions to Lua
	void RegisterLuaBindings();
};
