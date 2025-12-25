#pragma once
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <GameObject/GameObject.h>
#include <Component/TransformComponent.h>
#include <Component/MeshComponent.h>
#include <Graphics/Utils/Camera.h>
#include <PlayerObject/PlayerLuaKeyActionsMapping.h>

#include <Graphics/Renderer/DeferredRenderer.h>
#include <PlayerObject/MiniViewRenderer.h>

class PlayerObject : public GameObject
{
public:
	// camera settings
	eastl::shared_ptr<SE_G::Camera> m_playerCamera;
};


class PlayerObject_Info : public GameObject_Info
{
public:
	// camera settings
	eastl::shared_ptr<SE_G::Camera> m_playerCamera;
	// input mapping

	// MiniViewRenderer
	eastl::shared_ptr<SE_G::MiniViewRenderer> m_miniViewRenderer;

	// allowed components:
	// transform, mesh, physics?
	RenderComponent_Info* m_renderComp;
	TransformComponent_Info* m_transformComp;
	MeshComponent_Info* m_meshComp;
	PhysicsComponent_Info* m_physComp;
	
	PlayerObject_Info() : GameObject_Info()
	{

	};

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

	void SetUpCamera()
	{
		m_playerCamera = eastl::make_shared<SE_G::Camera>(
			m_miniViewRenderer->GetDevice(), 640.0f / 360.0f);
		m_playerCamera->SwitchToFollowMode(this);
		m_miniViewRenderer->SetMainCamera(m_playerCamera);
	}

	void RenderViewport()
	{
		m_miniViewRenderer->Pass();
	}


public:

	PlayerLuaKeyActionsMapping m_luaActionMapping;

};