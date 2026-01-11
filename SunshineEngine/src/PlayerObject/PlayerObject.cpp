#include <PlayerObject/PlayerObject.h>

#include <Graphics/Renderer/Technique/IconTechnique.h>

#include <Utils/AssetPath.h>
#include <Utils/StringUtils.h>

PlayerObject::PlayerObject(const json& j, SE_G::DeferredRenderer* renderSystem)
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
	
	SetupLuaActionMapping_test();
}

void PlayerObject::SetupLuaActionMapping_test()
{
	// In your PlayerObject constructor or initialization:
	AssetPath scriptPath(L"player_controller.lua", AssetPath::AssetSource::Project);

	m_luaActionMapping.Initialize(WStringToUtf8(scriptPath.GetFullPath()));
	m_luaActionMapping.SetPlayerObject(this);

	// Bind keys
	m_luaActionMapping.BindKey(Keys::Q, "onMoveForward");
	m_luaActionMapping.BindKey(Keys::E, "onJump");

	// Enable Lua mode
	m_playerController.SetLuaCallbackMode(true);
}

PlayerObject_Info::PlayerObject_Info() : GameObject_Info()
{
	m_name = "PlayerObject";
	m_group = GameObjectGroup::Player;
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
}