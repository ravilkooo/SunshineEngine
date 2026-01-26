#pragma once

#include <EASTL/string.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <Component/ComponentType.h>
#include <GameObject/ObjectGroupType.h>
#include <Graphics/Lighting/LightData.h>
#include <Utils/AssetPath.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Component_Prefab;

class Prefab
{
public:
	json m_prefabInfo;
	eastl::string m_prefabName = "";
	AssetPath m_prefabFilePath = AssetPath(L"", AssetPath::AssetSource::Project);

	Prefab();

	void Save();
};

class PrefabDescription
{
public:
	PrefabDescription(Prefab* prefab);

	GameObjectGroup m_group;
	ObjectType m_type;

	Prefab* m_assignedPrefab = nullptr;
	eastl::vector<Component_Prefab> m_components;

	void AddComponent(SE::ComponentType type);
	bool HasComponent(SE::ComponentType type);
	void RemoveComponent(SE::ComponentType type);


};

class LightData_Prefab
{
public:
	union
	{
		SE_G::AmbientLightData asAmbient;
		SE_G::DirectionalLightData asDir;
		SE_G::PointLightData asPoint;
		SE_G::SpotLightData asSpot;
		SE_G::SkyBoxData asSky;
	} m_lightData;

	AssetPath texPath;

	ObjectType m_lightType;
};
