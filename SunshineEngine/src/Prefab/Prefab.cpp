#include <Prefab/Prefab.h>

#include <Prefab/Component_Prefab.h>

Prefab::Prefab()
{
	m_prefabInfo = json();
}


PrefabDescription::PrefabDescription(Prefab* prefab)
	: m_assignedPrefab(prefab)
{
	if (!prefab)
		return;

	
}

void PrefabDescription::AddComponent(SE::ComponentType type)
{
	if (HasComponent(type))
		return;

	switch (type)
	{
	case SE::ComponentType::TRANSFORM:
		m_components.push_back(Transform_Prefab());
		break;

	case SE::ComponentType::LUA:
		m_components.push_back(Lua_Prefab());
		break;

	case SE::ComponentType::PHYSICS:
		m_components.push_back(Physics_Prefab());
		break;

	case SE::ComponentType::PERCEPTION:
		m_components.push_back(Perception_Prefab());
		break;

	case SE::ComponentType::TRIGGER:
		m_components.push_back(Trigger_Prefab());
		break;

	case SE::ComponentType::BEHAVIOR:
		m_components.push_back(Behavior_Prefab());
		break;

	case SE::ComponentType::MESH:
		m_components.push_back(Mesh_Prefab());
		break;

	case SE::ComponentType::PARTICLE_EMITTER:
		m_components.push_back(Emitter_Prefab());
		break;

	default:
		break;
	}
	//
}
bool PrefabDescription::HasComponent(SE::ComponentType type)
{
	return false;
	//
	//
}
void PrefabDescription::RemoveComponent(SE::ComponentType type)
{
	//
	//
}
