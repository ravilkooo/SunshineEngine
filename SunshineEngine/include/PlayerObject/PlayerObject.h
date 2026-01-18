#pragma once
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/vector.h>

#include <GameObject/GameObject.h>

#include <PlayerObject/PlayerController.h>
#include <PlayerObject/PlayerLuaKeyActionsMapping.h>
#include <PlayerObject/KeyFunctionPair.h>
#include <PlayerObject/MiniViewRenderer.h>

#include <Utils/AssetPath.h>

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
	bool m_fixedCamera = false;

	// Input
	PlayerController m_playerController;

	PlayerObject();

	PlayerObject(const json& j, SE_G::DeferredRenderer* renderSystem, eastl::shared_ptr<SE_G::Camera> camera);

	void SettingsFromJson(const json& j, eastl::shared_ptr<SE_G::Camera> camera);

	void SetUpCamera(SE_G::DeferredRenderer* renderSystem);

	void AssignSceneToCamera(Scene* scene);

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
	bool m_fixeCamera = false;

	// Lua script configuration
	AssetPath m_luaScriptPath;
	
	// input mapping: key -> lua function name
	eastl::vector<KeyFunctionPair> m_keyFunctionMapping;

	// Lua function for mouse args
	eastl::string m_mouseActionsHandlingFunction;

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
	void SetLuaScriptPath(const AssetPath& scriptPath);

	/**
	 * Get the Lua script path
	 */
	const AssetPath& GetLuaScriptPath() const;

	/**
	 * Check if a Lua script is set
	 */
	bool HasLuaScript() const;

	// ===== Key-Function Pair Management =====

	/**
	 * Add a new key-function binding pair
	 */
	void AddKeyFunctionPair(Keys key, const eastl::string& functionName);

	/**
	 * Add a key-function pair
	 */
	void AddKeyFunctionPair(const KeyFunctionPair& pair);

	/**
	 * Remove a key-function pair by index
	 */
	bool RemoveKeyFunctionPair(size_t index);

	/**
	 * Edit a key-function pair at specific index
	 */
	bool EditKeyFunctionPair(size_t index, Keys newKey, const eastl::string& newFunctionName);

	/**
	 * Edit key for a specific pair
	 */
	bool EditKeyFunctionPairKey(size_t index, Keys newKey);

	/**
	 * Edit function name for a specific pair
	 */
	bool EditKeyFunctionPairFunction(size_t index, const eastl::string& newFunctionName);

	/**
	 * Get a key-function pair by index
	 */
	const KeyFunctionPair* GetKeyFunctionPair(size_t index) const;

	/**
	 * Get all key-function pairs
	 */
	const eastl::vector<KeyFunctionPair>& GetAllKeyFunctionPairs() const;

	/**
	 * Get number of key-function pairs
	 */
	size_t GetKeyFunctionPairCount() const;

	/**
	 * Clear all key-function pairs
	 */
	void ClearKeyFunctionPairs();

	/**
	 * Find index of pair by key
	 */
	int FindPairIndexByKey(Keys key) const;

	void AddRenderComponent(SE_G::DeferredRenderer* renderSystem);
	void AddTransformComponent(ID3D11Device* device);
	void AddMeshComponent();
	void AddPhysicsComponent();

	void InitMiniViewport(SE_G::DeferredRenderer* defRenderer);
	void AssignSceneToCamera(Scene_Info* scene);
	void SetUpCamera();
	void RenderViewport();

public:

	PlayerLuaKeyActionsMapping m_luaActionMapping;

};