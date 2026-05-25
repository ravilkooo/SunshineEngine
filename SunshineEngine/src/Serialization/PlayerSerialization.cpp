#include <PlayerObject/PlayerObject.h>

#include <Serialization/GraphicsSerialization.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void PlayerObject::SettingsFromJson(const json& j, eastl::shared_ptr<SE_G::Camera> camera)
{
	if (j.contains("camera"))
	{
		if (j["camera"].contains("length")) {
			m_playerCamera->m_stickParams.length = j["camera"]["length"].get<float>();
		}
		if (j["camera"].contains("pitchYawRoll") && j["camera"]["pitchYawRoll"].is_array() && j["camera"]["pitchYawRoll"].size() >= 3) {
			m_playerCamera->m_stickParams.pitchYawRoll.x = j["camera"]["pitchYawRoll"][0].get<float>();
			m_playerCamera->m_stickParams.pitchYawRoll.y = j["camera"]["pitchYawRoll"][1].get<float>();
			m_playerCamera->m_stickParams.pitchYawRoll.z = j["camera"]["pitchYawRoll"][2].get<float>();
		}
		if (j["camera"].contains("rootOffset") && j["camera"]["rootOffset"].is_array() && j["camera"]["rootOffset"].size() >= 3) {
			m_playerCamera->m_stickParams.rootOffset.x = j["camera"]["rootOffset"][0].get<float>();
			m_playerCamera->m_stickParams.rootOffset.y = j["camera"]["rootOffset"][1].get<float>();
			m_playerCamera->m_stickParams.rootOffset.z = j["camera"]["rootOffset"][2].get<float>();
		}
	}

	if (j.contains("fixedCamera")) {
		m_fixedCamera = j["fixedCamera"].get<bool>();
	}

	// Load Lua script
	if (j.contains("luaScript") && j.contains("keyFunctionMappings"))
	{
		SetupLuaActionMapping(j);
	}
	else
	{
		SetDefaultLuaActionMapping();
	}
}

json PlayerObject_Info::ToJson() const {
	json j = GameObject_Info::ToJson();
	j["settings"] = SettingsToJson();
	return j;
}

json PlayerObject_Info::SettingsToJson() const
{
	// j["settings"] = json::object();
	json j;

	j["camera"] = json::object();
	j["camera"]["length"] = m_playerCamera->m_stickParams.length;
	j["camera"]["pitchYawRoll"] = {
		m_playerCamera->m_stickParams.pitchYawRoll.x,
		m_playerCamera->m_stickParams.pitchYawRoll.y,
		m_playerCamera->m_stickParams.pitchYawRoll.z
	};
	j["camera"]["rootOffset"] = {
		m_playerCamera->m_stickParams.rootOffset.x,
		m_playerCamera->m_stickParams.rootOffset.y,
		m_playerCamera->m_stickParams.rootOffset.z
	};

	// Serialize Lua script path
	j["luaScript"] = m_luaScriptPath.ToJson();

	// Serialize key-function mappings
	j["keyFunctionMappings"] = json::array();
	for (const auto& pair : m_keyFunctionMapping) {
		j["keyFunctionMappings"].push_back(pair.ToJson());
	}

	j["fixedCamera"] = m_fixedCamera;

	j["mouseFunctionMapping"] = m_mouseActionsHandlingFunction.c_str();

	return j;
}

void PlayerObject_Info::SettingsFromJson(const json& j, SE_G::DeferredRenderer* defRenderer)
{
	InitMiniViewport(defRenderer);
	SetUpCamera();

	if (j.contains("camera"))
	{
		if (j["camera"].contains("length")) {
			m_playerCamera->m_stickParams.length = j["camera"]["length"].get<float>();
		}
		if (j["camera"].contains("pitchYawRoll") && j["camera"]["pitchYawRoll"].is_array() && j["camera"]["pitchYawRoll"].size() >= 3) {
			m_playerCamera->m_stickParams.pitchYawRoll.x = j["camera"]["pitchYawRoll"][0].get<float>();
			m_playerCamera->m_stickParams.pitchYawRoll.y = j["camera"]["pitchYawRoll"][1].get<float>();
			m_playerCamera->m_stickParams.pitchYawRoll.z = j["camera"]["pitchYawRoll"][2].get<float>();
		}
		if (j["camera"].contains("rootOffset") && j["camera"]["rootOffset"].is_array() && j["camera"]["rootOffset"].size() >= 3) {
			m_playerCamera->m_stickParams.rootOffset.x = j["camera"]["rootOffset"][0].get<float>();
			m_playerCamera->m_stickParams.rootOffset.y = j["camera"]["rootOffset"][1].get<float>();
			m_playerCamera->m_stickParams.rootOffset.z = j["camera"]["rootOffset"][2].get<float>();
		}
	}

	if (j.contains("fixedCamera")) {
		m_fixedCamera = j["fixedCamera"].get<bool>();
	}
	
	// Load Lua script
	if (j.contains("luaScript")) {
		m_luaScriptPath.FromJson(j["luaScript"]);
	}

	// Load key-function mappings
	if (j.contains("keyFunctionMappings")) {
		m_keyFunctionMapping.clear();
		for (const auto& pairJson : j["keyFunctionMappings"]) {
			m_keyFunctionMapping.push_back(KeyFunctionPair::FromJson(pairJson));
		}
	}

	if (j.contains("mouseFunctionMapping")) {
		m_mouseActionsHandlingFunction = j["mouseFunctionMapping"].get<std::string>().c_str();
	}
}