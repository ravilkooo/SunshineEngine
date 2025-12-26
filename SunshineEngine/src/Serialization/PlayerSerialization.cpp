#include <PlayerObject/PlayerObject.h>

#include <Serialization/GraphicsSerialization.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void PlayerObject::SettingsFromJson(const json& j, eastl::shared_ptr<SE_G::Camera> camera)
{
	m_playerCamera = camera;
	m_playerCamera->SetFollowPlayer(m_UUID);

	if (j.contains("camera"))
	{
		if (j["camera"].contains("stickLength")) {
			m_playerCamera->m_stickParams.stickLength = j["camera"]["stickLength"].get<float>();
		}
		if (j["camera"].contains("stickYaw")) {
			m_playerCamera->m_stickParams.stickYaw = j["camera"]["stickYaw"].get<float>();
		}
		if (j["camera"].contains("stickPitch")) {
			m_playerCamera->m_stickParams.stickPitch = j["camera"]["stickPitch"].get<float>();
		}
		if (j["camera"].contains("viewPitchYawRoll") && j["camera"]["viewPitchYawRoll"].is_array() && j["camera"]["viewPitchYawRoll"].size() >= 3) {
			m_playerCamera->m_stickParams.viewPitchYawRoll.x = j["camera"]["viewPitchYawRoll"][0].get<float>();
			m_playerCamera->m_stickParams.viewPitchYawRoll.y = j["camera"]["viewPitchYawRoll"][1].get<float>();
			m_playerCamera->m_stickParams.viewPitchYawRoll.z = j["camera"]["viewPitchYawRoll"][2].get<float>();
		}
		if (j["camera"].contains("offset") && j["camera"]["offset"].is_array() && j["camera"]["offset"].size() >= 3) {
			m_playerCamera->m_stickParams.offset.x = j["camera"]["offset"][0].get<float>();
			m_playerCamera->m_stickParams.offset.y = j["camera"]["offset"][1].get<float>();
			m_playerCamera->m_stickParams.offset.z = j["camera"]["offset"][2].get<float>();
		}
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
	j["camera"]["stickLength"] = m_playerCamera->m_stickParams.stickLength;
	j["camera"]["stickYaw"] = m_playerCamera->m_stickParams.stickYaw;
	j["camera"]["stickPitch"] = m_playerCamera->m_stickParams.stickPitch;
	j["camera"]["viewPitchYawRoll"] = {
		m_playerCamera->m_stickParams.viewPitchYawRoll.x,
		m_playerCamera->m_stickParams.viewPitchYawRoll.y,
		m_playerCamera->m_stickParams.viewPitchYawRoll.z
	};
	j["camera"]["offset"] = {
		m_playerCamera->m_stickParams.offset.x,
		m_playerCamera->m_stickParams.offset.y,
		m_playerCamera->m_stickParams.offset.z
	};

	return j;
}

void PlayerObject_Info::SettingsFromJson(const json& j, SE_G::DeferredRenderer* defRenderer)
{
	InitMiniViewport(defRenderer);
	SetUpCamera();

	if (j.contains("camera"))
	{
		if (j["camera"].contains("stickLength")) {
			m_playerCamera->m_stickParams.stickLength = j["camera"]["stickLength"].get<float>();
		}
		if (j["camera"].contains("stickYaw")) {
			m_playerCamera->m_stickParams.stickYaw = j["camera"]["stickYaw"].get<float>();
		}
		if (j["camera"].contains("stickPitch")) {
			m_playerCamera->m_stickParams.stickPitch = j["camera"]["stickPitch"].get<float>();
		}
		if (j["camera"].contains("viewPitchYawRoll") && j["camera"]["viewPitchYawRoll"].is_array() && j["camera"]["viewPitchYawRoll"].size() >= 3) {
			m_playerCamera->m_stickParams.viewPitchYawRoll.x = j["camera"]["viewPitchYawRoll"][0].get<float>();
			m_playerCamera->m_stickParams.viewPitchYawRoll.y = j["camera"]["viewPitchYawRoll"][1].get<float>();
			m_playerCamera->m_stickParams.viewPitchYawRoll.z = j["camera"]["viewPitchYawRoll"][2].get<float>();
		}
		if (j["camera"].contains("offset") && j["camera"]["offset"].is_array() && j["camera"]["offset"].size() >= 3) {
			m_playerCamera->m_stickParams.offset.x = j["camera"]["offset"][0].get<float>();
			m_playerCamera->m_stickParams.offset.y = j["camera"]["offset"][1].get<float>();
			m_playerCamera->m_stickParams.offset.z = j["camera"]["offset"][2].get<float>();
		}
	}
}