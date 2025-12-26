#include <PlayerObject/PlayerObject.h>

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
		GetComponent<RenderComponent_Info>().get(),
		GetComponent<TransformComponent_Info>().get()).get();

	if (j["components"].contains("Physics"))
	{
		m_physComp->FromJson(j["components"]["Physics"]);
	}
}