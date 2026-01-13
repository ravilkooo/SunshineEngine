#include <PlayerObject/PlayerObject.h>

#include <Graphics/Renderer/Technique/IconTechnique.h>

#include <Utils/AssetPath.h>
#include <Utils/StringUtils.h>

PlayerObject::PlayerObject(const json& j, SE_G::DeferredRenderer* renderSystem, eastl::shared_ptr<SE_G::Camera> camera)
{
	m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	m_name = "PlayerObject";
	//m_group = GameObjectGroup::Player;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc = AddComponent<TransformComponent>(device).get();
	if (j["components"].contains("Transform")) {
		tc->FromJson(j["components"]["Transform"]);
	}

	auto rc = AddComponent<RenderComponent>(m_UUID, renderSystem).get();

	auto mc = AddComponent<MeshComponent>().get();
	mc->FromJson(
		j["components"]["Mesh"],
		device, rc, tc, m_UUID);

	m_playerController.SetPlayerObject(this);

	m_playerCamera = camera;
	m_playerCamera->SetFollowPlayer(m_UUID);

	if (j.contains("settings"))
	{
		SettingsFromJson(j["settings"], camera);
	}
	else
	{
		json _empty;
		SettingsFromJson(_empty, camera);
	}
}

void PlayerObject::SetDefaultLuaActionMapping()
{
	// In your PlayerObject constructor or initialization:
	AssetPath scriptPath(L"Scripts/player_controller.lua", AssetPath::AssetSource::Project);

	m_luaActionMapping.Initialize(scriptPath);
	m_luaActionMapping.SetPlayerObject(this);

	// Bind keys
	m_luaActionMapping.BindKey(Keys::Q, "onMoveForward");
	m_luaActionMapping.BindKey(Keys::E, "onJump");

	// Enable Lua mode
	m_playerController.SetLuaCallbackMode(true);
}

void PlayerObject::SetupLuaActionMapping(const json& j)
{
	AssetPath scriptPath; scriptPath.FromJson(j["luaScript"]);

	m_luaActionMapping.Initialize(scriptPath);
	m_luaActionMapping.SetPlayerObject(this);

	// Bind keys
	m_luaActionMapping.InitBindingFromJson(j["keyFunctionMappings"]);

	// Enable Lua mode
	m_playerController.SetLuaCallbackMode(true);
}

PlayerObject_Info::PlayerObject_Info() : GameObject_Info()
{
	m_name = "PlayerObject";
	m_group = GameObjectGroup::Player;
	
	m_luaScriptPath = AssetPath();
	m_keyFunctionMapping = eastl::vector<KeyFunctionPair>();
};

PlayerObject_Info::PlayerObject_Info(SE_G::DeferredRenderer* renderSystem) : GameObject_Info()
{
	m_name = "PlayerObject";
	m_group = GameObjectGroup::Player;

	auto tc_info = AddComponent<TransformComponent_Info>(renderSystem->GetDevice());
	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

	auto iconTech =
		eastl::make_unique<SE_G::IconTechnique>(renderSystem->GetDevice(),
			tc_info->m_assignedComponent.get(), eastl::string("IconPass"),
			SE_G::IconData{ 4u, 0u, 1u, 1u, m_UUID.GetHilo() });

	m_iconTech = static_cast<SE_G::IconTechnique*>(rc_info->AddTechnique(eastl::move(iconTech)));
	
	m_luaScriptPath = AssetPath();
	m_keyFunctionMapping = eastl::vector<KeyFunctionPair>();
};

PlayerObject_Info::PlayerObject_Info(const json& j, SE_G::DeferredRenderer* renderSystem)
{
	m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	m_name = "PlayerObject";
	m_group = GameObjectGroup::Player;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	m_transformComp = AddComponent<TransformComponent_Info>(device).get();
	if (j["components"].contains("Transform")) {
		m_transformComp->FromJson(j["components"]["Transform"], device);
	}

	m_renderComp = AddComponent<RenderComponent_Info>(m_UUID, renderSystem).get();

	auto iconTech =
		eastl::make_unique<SE_G::IconTechnique>(renderSystem->GetDevice(),
			m_transformComp->m_assignedComponent.get(), eastl::string("IconPass"),
			SE_G::IconData{ 4u, 0u, 1u, 1u, m_UUID.GetHilo() });

	m_iconTech = static_cast<SE_G::IconTechnique*>(m_renderComp->AddTechnique(eastl::move(iconTech)));

	m_meshComp = AddComponent<MeshComponent_Info>().get();
	m_meshComp->FromJson(
		j["components"]["Mesh"],
		device, m_renderComp, m_transformComp, m_UUID);

	m_physComp = AddComponent<PhysicsComponent_Info>(
		m_renderComp, m_transformComp).get();

	if (j["components"].contains("Physics"))
	{
		m_physComp->FromJson(j["components"]["Physics"]);
	}
	
	if (j.contains("settings"))
	{
		SettingsFromJson(j["settings"], renderSystem);
	}
	else
	{
		json _empty;
		SettingsFromJson(_empty, renderSystem);
	}
}