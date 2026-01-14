#pragma once
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/vector.h>

#include <GameObject/GameObject.h>
#include <Component/TransformComponent.h>
#include <Component/MeshComponent.h>
#include <Graphics/Utils/Camera.h>
#include <PlayerObject/PlayerLuaKeyActionsMapping.h>
#include <PlayerObject/KeyFunctionPair.h>
#include <Utils/AssetPath.h>

#include <Graphics/Renderer/DeferredRenderer.h>
#include <PlayerObject/MiniViewRenderer.h>
#include <PlayerObject/PlayerController.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace SE_G
{
	class IconTechnique;
}

class PlayerObject : public GameObject
{
public:
	// camera settings
	eastl::shared_ptr<SE_G::Camera> m_playerCamera;

	// Input
	PlayerController m_playerController;

	PlayerObject() : GameObject()
	{
		m_name = "PlayerObject";
	};

	PlayerObject(const json& j, SE_G::DeferredRenderer* renderSystem, eastl::shared_ptr<SE_G::Camera> camera);

	void SettingsFromJson(const json& j, eastl::shared_ptr<SE_G::Camera> camera);

	void SetUpCamera(SE_G::DeferredRenderer* renderSystem)
	{
		m_playerCamera = eastl::make_shared<SE_G::Camera>(
			renderSystem->GetDevice(), renderSystem->m_screenWidth / renderSystem->m_screenHeight);
		m_playerCamera->SetFollowPlayer(m_UUID);
	}

	void AssignSceneToCamera(Scene* scene)
	{
		m_playerCamera->AssignScene(scene);
	}

	void SettingsFromJson(const json& j, SE_G::DeferredRenderer* defRenderer);

	void SetDefaultLuaActionMapping();
	void SetupLuaActionMapping(const json& j);

	PlayerLuaKeyActionsMapping m_luaActionMapping;
};

class PlayerObject_Info : public GameObject_Info
{
public:
	// camera settings
	eastl::shared_ptr<SE_G::Camera> m_playerCamera;

	// Lua script configuration
	AssetPath m_luaScriptPath;
	
	// input mapping: key -> lua function name
	eastl::vector<KeyFunctionPair> m_keyFunctionMapping;

	// MiniViewRenderer
	eastl::shared_ptr<SE_G::MiniViewRenderer> m_miniViewRenderer;

	// allowed components:
	// transform, mesh, physics?
	RenderComponent_Info* m_renderComp;
	TransformComponent_Info* m_transformComp;
	MeshComponent_Info* m_meshComp;
	PhysicsComponent_Info* m_physComp;
	SE_G::IconTechnique* m_iconTech;
	
	PlayerObject_Info();
	PlayerObject_Info(SE_G::DeferredRenderer* renderSystem);

	PlayerObject_Info(const json& j, SE_G::DeferredRenderer* renderSystem);

	virtual json ToJson() const override;

	json SettingsToJson() const;
	void SettingsFromJson(const json& j, SE_G::DeferredRenderer* defRenderer);

	// ===== Lua Script Management =====

	/**
	 * Set the Lua script path
	 */
	void SetLuaScriptPath(const AssetPath& scriptPath)
	{
		m_luaScriptPath = scriptPath;
	}

	/**
	 * Get the Lua script path
	 */
	const AssetPath& GetLuaScriptPath() const
	{
		return m_luaScriptPath;
	}

	/**
	 * Check if a Lua script is set
	 */
	bool HasLuaScript() const
	{
		return !m_luaScriptPath.GetFullPath().empty();
	}

	// ===== Key-Function Pair Management =====

	/**
	 * Add a new key-function binding pair
	 */
	void AddKeyFunctionPair(Keys key, const eastl::string& functionName)
	{
		if (key != Keys::None && !functionName.empty()) {
			m_keyFunctionMapping.push_back(KeyFunctionPair(key, functionName));
		}
	}

	/**
	 * Add a key-function pair
	 */
	void AddKeyFunctionPair(const KeyFunctionPair& pair)
	{
		if (pair.IsValid()) {
			m_keyFunctionMapping.push_back(pair);
		}
	}

	/**
	 * Remove a key-function pair by index
	 */
	bool RemoveKeyFunctionPair(size_t index)
	{
		if (index < m_keyFunctionMapping.size()) {
			m_keyFunctionMapping.erase(m_keyFunctionMapping.begin() + index);
			return true;
		}
		return false;
	}

	/**
	 * Edit a key-function pair at specific index
	 */
	bool EditKeyFunctionPair(size_t index, Keys newKey, const eastl::string& newFunctionName)
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

	/**
	 * Edit key for a specific pair
	 */
	bool EditKeyFunctionPairKey(size_t index, Keys newKey)
	{
		if (index < m_keyFunctionMapping.size() && newKey != Keys::None) {
			m_keyFunctionMapping[index].key = newKey;
			return true;
		}
		return false;
	}

	/**
	 * Edit function name for a specific pair
	 */
	bool EditKeyFunctionPairFunction(size_t index, const eastl::string& newFunctionName)
	{
		if (index < m_keyFunctionMapping.size() && !newFunctionName.empty()) {
			m_keyFunctionMapping[index].functionName = newFunctionName;
			return true;
		}
		return false;
	}

	/**
	 * Get a key-function pair by index
	 */
	const KeyFunctionPair* GetKeyFunctionPair(size_t index) const
	{
		if (index < m_keyFunctionMapping.size()) {
			return &m_keyFunctionMapping[index];
		}
		return nullptr;
	}

	/**
	 * Get all key-function pairs
	 */
	const eastl::vector<KeyFunctionPair>& GetAllKeyFunctionPairs() const
	{
		return m_keyFunctionMapping;
	}

	/**
	 * Get number of key-function pairs
	 */
	size_t GetKeyFunctionPairCount() const
	{
		return m_keyFunctionMapping.size();
	}

	/**
	 * Clear all key-function pairs
	 */
	void ClearKeyFunctionPairs()
	{
		m_keyFunctionMapping.clear();
	}

	/**
	 * Find index of pair by key
	 */
	int FindPairIndexByKey(Keys key) const
	{
		for (size_t i = 0; i < m_keyFunctionMapping.size(); ++i) {
			if (m_keyFunctionMapping[i].key == key) {
				return static_cast<int>(i);
			}
		}
		return -1;
	}

	void AddRenderComponent(SE_G::DeferredRenderer* renderSystem)
	{
		m_renderComp = this->AddComponent<RenderComponent_Info>(this->m_UUID, renderSystem).get();
	};

	void AddTransformComponent(ID3D11Device* device)
	{
		m_transformComp = this->AddComponent<TransformComponent_Info>(device).get();
	};

	void AddMeshComponent()
	{
		auto meshPtr = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(
			m_renderComp->GetDevice(),
			DXSM::Vector3(1.0f, 1.0f, 1.0f)
		);
		m_meshComp = this->AddComponent<MeshComponent_Info>(
			m_renderComp, m_transformComp, this->m_UUID, meshPtr).get();
	};

	void AddPhysicsComponent()
	{
		m_physComp = this->AddComponent<PhysicsComponent_Info>(
			m_renderComp, m_transformComp).get();

		m_physComp->SetMotion(SE::PhysicsMotionType::Kinematic);
	};

	void InitMiniViewport(SE_G::DeferredRenderer* defRenderer)
	{
		m_miniViewRenderer = eastl::make_shared<SE_G::MiniViewRenderer>(
			"PlayerViewport", defRenderer->GetDevice(), defRenderer->GetDeviceContext());
		m_miniViewRenderer->SetParentRenderer(defRenderer);
		m_miniViewRenderer->Disable();
	}
	
	void AssignSceneToCamera(Scene_Info* scene)
	{
		m_playerCamera->AssignScene(scene);
	}

	void SetUpCamera()
	{
		m_playerCamera = eastl::make_shared<SE_G::Camera>(
			m_miniViewRenderer->GetDevice(), 640.0f / 360.0f);
		m_playerCamera->SetFollowPlayer(m_UUID);
		m_miniViewRenderer->SetMainCamera(m_playerCamera);
	}

	void RenderViewport()
	{
		m_miniViewRenderer->Pass();
	}


public:

	PlayerLuaKeyActionsMapping m_luaActionMapping;

};