#include <PlayerObject/PlayerObject.h>

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

	auto pc = AddComponent<PhysicsComponent>(
		m_UUID, tc).get();

	if (j["components"].contains("Physics"))
	{
		pc->FromJson(j["components"]["Physics"]);
	}
}

PlayerObject_Info::PlayerObject_Info() : GameObject_Info()
{
	m_name = "PlayerObject";
	m_group = GameObjectGroup::Player;
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