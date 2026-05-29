#include <PlayerObject/PlayerObject.h>

#include <Serialization/GraphicsSerialization.h>
#include <Graphics/Utils/Camera.h>
#include <Component/TransformComponent.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void PlayerObject::SettingsFromJson(const json& j)
{
	/*
	if (j.contains("camera"))
	{
		if (j["camera"].contains("m_springArmParams"))
		{
			if (j["camera"]["m_springArmParams"].contains("length")) {
				m_playerCamera->m_springArmParams.length = j["camera"]["m_springArmParams"]["length"].get<float>();
			}
			if (j["camera"]["m_springArmParams"].contains("pitchYawRoll") && j["camera"]["m_springArmParams"]["pitchYawRoll"].is_array() && j["camera"]["m_springArmParams"]["pitchYawRoll"].size() >= 3) {
				m_playerCamera->m_springArmParams.pitchYawRoll.x = j["camera"]["m_springArmParams"]["pitchYawRoll"][0].get<float>();
				m_playerCamera->m_springArmParams.pitchYawRoll.y = j["camera"]["m_springArmParams"]["pitchYawRoll"][1].get<float>();
				m_playerCamera->m_springArmParams.pitchYawRoll.z = j["camera"]["m_springArmParams"]["pitchYawRoll"][2].get<float>();
			}
			if (j["camera"]["m_springArmParams"].contains("rootOffset") && j["camera"]["m_springArmParams"]["rootOffset"].is_array() && j["camera"]["m_springArmParams"]["rootOffset"].size() >= 3) {
				m_playerCamera->m_springArmParams.rootOffset.x = j["camera"]["m_springArmParams"]["rootOffset"][0].get<float>();
				m_playerCamera->m_springArmParams.rootOffset.y = j["camera"]["m_springArmParams"]["rootOffset"][1].get<float>();
				m_playerCamera->m_springArmParams.rootOffset.z = j["camera"]["m_springArmParams"]["rootOffset"][2].get<float>();
			}
		}
		if (j["camera"].contains("cameraPitchYawRoll") && j["camera"]["cameraPitchYawRoll"].is_array() && j["camera"]["cameraPitchYawRoll"].size() >= 3) {
			m_playerCamera->cameraPitchYawRoll.x = j["camera"]["cameraPitchYawRoll"][0].get<float>();
			m_playerCamera->cameraPitchYawRoll.y = j["camera"]["cameraPitchYawRoll"][1].get<float>();
			m_playerCamera->cameraPitchYawRoll.z = j["camera"]["cameraPitchYawRoll"][2].get<float>();
		}
	}
	*/

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

	/*
	j["camera"] = json::object();
	j["camera"]["cameraPitchYawRoll"] = {
		m_playerCamera->cameraPitchYawRoll.x,
		m_playerCamera->cameraPitchYawRoll.y,
		m_playerCamera->cameraPitchYawRoll.z
	};

	j["camera"]["m_springArmParams"] = json::object();
	j["camera"]["m_springArmParams"]["length"] = m_playerCamera->m_springArmParams.length;
	j["camera"]["m_springArmParams"]["pitchYawRoll"] = {
		m_playerCamera->m_springArmParams.pitchYawRoll.x,
		m_playerCamera->m_springArmParams.pitchYawRoll.y,
		m_playerCamera->m_springArmParams.pitchYawRoll.z
	};
	j["camera"]["m_springArmParams"]["rootOffset"] = {
		m_playerCamera->m_springArmParams.rootOffset.x,
		m_playerCamera->m_springArmParams.rootOffset.y,
		m_playerCamera->m_springArmParams.rootOffset.z
	};
	*/

	// Serialize Lua script path
	j["luaScript"] = m_luaScriptPath.ToJson();

	// Serialize key-function mappings
	j["keyFunctionMappings"] = json::array();
	for (const auto& pair : m_keyFunctionMapping) {
		j["keyFunctionMappings"].push_back(pair.ToJson());
	}

	j["mouseFunctionMapping"] = m_mouseActionsHandlingFunction.c_str();

	return j;
}

void PlayerObject_Info::SettingsFromJson(const json& j, SE_G::DeferredRenderer* defRenderer)
{
	/*
	SetUpCamera(defRenderer);

	if (j.contains("camera"))
	{
		if (j["camera"].contains("m_springArmParams"))
		{
			if (j["camera"]["m_springArmParams"].contains("length")) {
				m_playerCamera->m_springArmParams.length = j["camera"]["m_springArmParams"]["length"].get<float>();
			}
			if (j["camera"]["m_springArmParams"].contains("pitchYawRoll") && j["camera"]["m_springArmParams"]["pitchYawRoll"].is_array() && j["camera"]["m_springArmParams"]["pitchYawRoll"].size() >= 3) {
				m_playerCamera->m_springArmParams.pitchYawRoll.x = j["camera"]["m_springArmParams"]["pitchYawRoll"][0].get<float>();
				m_playerCamera->m_springArmParams.pitchYawRoll.y = j["camera"]["m_springArmParams"]["pitchYawRoll"][1].get<float>();
				m_playerCamera->m_springArmParams.pitchYawRoll.z = j["camera"]["m_springArmParams"]["pitchYawRoll"][2].get<float>();
			}
			if (j["camera"]["m_springArmParams"].contains("rootOffset") && j["camera"]["m_springArmParams"]["rootOffset"].is_array() && j["camera"]["m_springArmParams"]["rootOffset"].size() >= 3) {
				m_playerCamera->m_springArmParams.rootOffset.x = j["camera"]["m_springArmParams"]["rootOffset"][0].get<float>();
				m_playerCamera->m_springArmParams.rootOffset.y = j["camera"]["m_springArmParams"]["rootOffset"][1].get<float>();
				m_playerCamera->m_springArmParams.rootOffset.z = j["camera"]["m_springArmParams"]["rootOffset"][2].get<float>();
			}
		}
		if (j["camera"].contains("cameraPitchYawRoll") && j["camera"]["cameraPitchYawRoll"].is_array() && j["camera"]["cameraPitchYawRoll"].size() >= 3) {
			m_playerCamera->cameraPitchYawRoll.x = j["camera"]["cameraPitchYawRoll"][0].get<float>();
			m_playerCamera->cameraPitchYawRoll.y = j["camera"]["cameraPitchYawRoll"][1].get<float>();
			m_playerCamera->cameraPitchYawRoll.z = j["camera"]["cameraPitchYawRoll"][2].get<float>();
		}
	}
	*/

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