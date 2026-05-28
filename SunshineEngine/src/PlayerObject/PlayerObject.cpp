#include <PlayerObject/PlayerObject.h>

#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>
#include <Graphics/Utils/Camera.h>

#include <Component/RenderComponent.h>
#include <Component/PhysicsComponent.h>
#include <Component/TransformComponent.h>
#include <Component/MeshComponent.h>
#include <Component/CameraComponent.h>

#include <ResourceManager/ResourceManagerFacade.h>
#include <Graphics/GraphicsResources/Mesh.h>

#include <Scene.h>

#include <Utils/AssetPath.h>
#include <Utils/StringUtils.h>


PlayerObject::PlayerObject() : GameObject()
{
	m_name = "PlayerObject";
};

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
	m_playerCamera->SetFollowUUID(m_UUID);

	if (j.contains("settings"))
	{
		SettingsFromJson(j["settings"]);
	}
	else
	{
		json _empty;
		SettingsFromJson(_empty);
	}

	AddComponent<CameraComponent>(m_playerCamera, tc, m_UUID);
}

void PlayerObject::SetUpCamera(SE_G::DeferredRenderer* renderSystem)
{
	m_playerCamera = eastl::make_shared<SE_G::Camera>(
		renderSystem->GetDevice(), renderSystem->m_screenWidth / renderSystem->m_screenHeight);
	m_playerCamera->AssignTransformComponent(GetComponent<TransformComponent>().get());
	m_playerCamera->SetFollowUUID(m_UUID);
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
	m_luaActionMapping.InitKeyBindingFromJson(j["keyFunctionMappings"]);
	m_luaActionMapping.InitMouseActionHandler(j["mouseFunctionMapping"].get<std::string>().c_str());

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

	SetUpCamera(renderSystem);
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

void PlayerObject_Info::SetLuaScriptPath(const AssetPath& scriptPath)
{
	m_luaScriptPath = scriptPath;
}

const AssetPath& PlayerObject_Info::GetLuaScriptPath() const
{
	return m_luaScriptPath;
}

bool PlayerObject_Info::HasLuaScript() const
{
	return !m_luaScriptPath.GetFullPath().empty();
}


void PlayerObject_Info::AddKeyFunctionPair(Keys key, const eastl::string& functionName)
{
	if (key != Keys::None && !functionName.empty()) {
		m_keyFunctionMapping.push_back(KeyFunctionPair(key, functionName));
	}
}

void PlayerObject_Info::AddKeyFunctionPair(const KeyFunctionPair& pair)
{
	if (pair.IsValid()) {
		m_keyFunctionMapping.push_back(pair);
	}
}

bool PlayerObject_Info::RemoveKeyFunctionPair(size_t index)
{
	if (index < m_keyFunctionMapping.size()) {
		m_keyFunctionMapping.erase(m_keyFunctionMapping.begin() + index);
		return true;
	}
	return false;
}

bool PlayerObject_Info::EditKeyFunctionPair(size_t index, Keys newKey, const eastl::string& newFunctionName)
{
	if (index < m_keyFunctionMapping.size()) {
		if (newKey != Keys::None && !newFunctionName.empty()) {
			m_keyFunctionMapping[index].key = newKey;
			m_keyFunctionMapping[index].functionName = newFunctionName;
			return true;
		}
	}
	return false;
}

bool PlayerObject_Info::EditKeyFunctionPairKey(size_t index, Keys newKey)
{
	if (index < m_keyFunctionMapping.size() && newKey != Keys::None) {
		m_keyFunctionMapping[index].key = newKey;
		return true;
	}
	return false;
}

bool PlayerObject_Info::EditKeyFunctionPairFunction(size_t index, const eastl::string& newFunctionName)
{
	if (index < m_keyFunctionMapping.size() && !newFunctionName.empty()) {
		m_keyFunctionMapping[index].functionName = newFunctionName;
		return true;
	}
	return false;
}

const KeyFunctionPair* PlayerObject_Info::GetKeyFunctionPair(size_t index) const
{
	if (index < m_keyFunctionMapping.size()) {
		return &m_keyFunctionMapping[index];
	}
	return nullptr;
}

const eastl::vector<KeyFunctionPair>& PlayerObject_Info::GetAllKeyFunctionPairs() const
{
	return m_keyFunctionMapping;
}

size_t PlayerObject_Info::GetKeyFunctionPairCount() const
{
	return m_keyFunctionMapping.size();
}

void PlayerObject_Info::ClearKeyFunctionPairs()
{
	m_keyFunctionMapping.clear();
}

int PlayerObject_Info::FindPairIndexByKey(Keys key) const
{
	for (size_t i = 0; i < m_keyFunctionMapping.size(); ++i) {
		if (m_keyFunctionMapping[i].key == key) {
			return static_cast<int>(i);
		}
	}
	return -1;
}

void PlayerObject_Info::AddRenderComponent(SE_G::DeferredRenderer* renderSystem)
{
	m_renderComp = this->AddComponent<RenderComponent_Info>(this->m_UUID, renderSystem).get();
};

void PlayerObject_Info::AddTransformComponent(ID3D11Device* device)
{
	m_transformComp = this->AddComponent<TransformComponent_Info>(device).get();
};

void PlayerObject_Info::AddMeshComponent()
{
	eastl::shared_ptr<SE_G::Mesh> meshPtr;
	AssetPath meshPath = AssetPath(L"Box_repeat");
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	meshPtr = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	meshPtr->m_meshPath = meshRes->m_meshPath;

	m_meshComp = this->AddComponent<MeshComponent_Info>(
		m_renderComp, m_transformComp, this->m_UUID, meshPtr).get();
};

void PlayerObject_Info::AddPhysicsComponent()
{
	m_physComp = this->AddComponent<PhysicsComponent_Info>(
		m_renderComp, m_transformComp).get();

	m_physComp->SetMotion(SE::PhysicsMotionType::Kinematic);
};

void PlayerObject_Info::SetUpCamera(SE_G::DeferredRenderer* defRenderer)
{
	m_playerCamera = eastl::make_shared<SE_G::Camera>(
		defRenderer->GetDevice(), 640.0f / 360.0f);
	m_playerCamera->AssignTransformComponent(m_transformComp->m_assignedComponent.get());
	m_playerCamera->SetFollowUUID(m_UUID);
}

