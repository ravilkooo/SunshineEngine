#include "WorldEditor.h"
#include <Component/LuaComponent.h>


WorldEditor::WorldEditor()
{
}

WorldEditor::~WorldEditor()
{
}


void WorldEditor::InitWorldEditor(
	eastl::shared_ptr<DeferredRenderer> renderer,
	UINT screenWidth,
	UINT screenHeight)
{
	this->m_renderer = renderer;
	this->m_screenHeight = screenHeight;
	this->m_screenWidth = screenWidth;

	{
		m_gPass = eastl::make_shared<GPass>(
			m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(m_gPass);
	}
	{
		m_lightPass = eastl::make_shared<LightPass>(
			m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(m_lightPass);
	}
	{
		m_selectionPass = eastl::make_shared<SelectionPass>(
			m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(m_selectionPass);
	}

	m_pixelUUIDHandler = new PixelUUIDHandler();
	m_pixelUUIDHandler->Init(m_renderer->GetDevice());

	/*
	TestObjects
	*/

	GameObjectFactory factory;

	m_scene.AddGameObject(factory.CreateSkyBox(
		m_renderer->GetDevice(),
		m_renderer->GetMainCamera())
	);
	m_scene.AddGameObject(factory.CreateDefaultBoxObject(
		m_renderer->GetDevice())
	);

	for (size_t i = 0; i < 6; i++)
	{
		m_scene.AddGameObject(factory.CreateDefaultSphereObject(
			m_renderer->GetDevice())
		);
		auto obj = m_scene.GetGameObjectByUUID(m_scene.gameObjects.back());
		auto tr = obj->GetComponent<TransformComponent>();
		tr->m_localPosition = DXSM::Vector3(-3.0f, 0.0f, 0.0f);
		tr->m_rotation.z = DX::XM_2PI * i / 6.0f;

	}

	m_scene.AddGameObject(factory.CreateAmbientLightObject(
		m_renderer->GetDevice(),
		m_renderer->GetMainCamera(),
		{ DXSM::Vector3::One * 0.5f, 1.0f })
	);
	m_scene.AddGameObject(factory.CreateDirectionalLightObject(
		m_renderer->GetDevice(),
		m_renderer->GetMainCamera(),
		{
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
			DXSM::Vector3::Zero, 0,
			DXSM::Vector3(1, -2, 0.5), 0
		})
	);
	m_scene.AddGameObject(factory.CreatePointLightObject(
		m_renderer->GetDevice(),
		m_renderer->GetMainCamera(),
		{
			DXSM::Vector3(0.0f, 0.0f, 1.0f), 1.0f,
			DXSM::Vector3(0.0f, 0.0f, 1.0f), 1.0f,
			DXSM::Vector3(1.0f, 0.0f, 0.0f), 20,
			DXSM::Vector3::One, 0
		})
	);
}
void WorldEditor::Run() {
	
}

void WorldEditor::Update(float deltaTime) {

	m_luaManager.Update(m_scene, deltaTime);
	//m_scene.gameObjects[1]->GetComponent<TransformComponent>()->m_localRotation.y += deltaTime;
	// m_scene.gameObjects[1]->GetComponent<TransformComponent>()->m_position.x += rayDirection.x * deltaTime * 10.0f;
	// m_scene.gameObjects[1]->GetComponent<TransformComponent>()->m_position.y += rayDirection.y * deltaTime * 10.0f;
	// m_scene.gameObjects[1]->GetComponent<TransformComponent>()->m_position.z += rayDirection.z * deltaTime * 10.0f;
}

void WorldEditor::Render() {
	
}

void WorldEditor::OnResize(UINT resizeWidth, UINT resizeHeight) {
	//m_renderer->GetMainCamera()->SetUpCameraViewByAspectRatio(m_screenWidth * 1.0f / m_screenHeight);
	if (resizeHeight == m_screenHeight)
		m_renderer->GetMainCamera()->SetUpCameraViewByAspectRatio_horizontal(resizeWidth * 1.0f / resizeHeight);
	else if (resizeWidth == m_screenWidth)
		m_renderer->GetMainCamera()->SetUpCameraViewByAspectRatio_vertical(resizeWidth * 1.0f / resizeHeight);
	else
		m_renderer->GetMainCamera()->ResetCameraView(resizeWidth * 1.0f / resizeHeight);

	m_screenWidth = resizeWidth;
	m_screenHeight = resizeHeight;
	
	m_lightPass->m_screenWidth = resizeWidth;
	m_lightPass->m_screenHeight = resizeHeight;

	m_gPass->OnResize(resizeWidth, resizeHeight);
	m_lightPass->OnResize(resizeWidth, resizeHeight, m_renderer->pGBuffer);
	m_selectionPass->OnResize(resizeWidth, resizeHeight, m_renderer->pGBuffer);
}

void WorldEditor::DeprojectScreenToWorld(DXSM::Vector2 mouseScreenCoords, DXSM::Vector2 lastGameViewportSize)
{
	float x = (2.0f * mouseScreenCoords.x) / lastGameViewportSize.x - 1.0f;
	float y = 1.0f - (2.0f * mouseScreenCoords.y) / lastGameViewportSize.y;
	float z = 0.0f;
	//printf("%f, %f :: %f, %f\n", mouseScreenCoords.x, mouseScreenCoords.y, lastGameViewportSize.x, lastGameViewportSize.y);

	DXSM::Vector4 ndcPosition(x, y, z, 1.0f);

	DXSM::Matrix viewProjMatrix =
		m_renderer->GetMainCamera()->GetViewMatrix() *
		m_renderer->GetMainCamera()->GetProjectionMatrix();
	DX::XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewProjMatrix);
	DXSM::Vector4 worldPos = XMVector3TransformCoord(ndcPosition, invViewProj);

	DXSM::Vector4 ndcPositionFar(x, y, 1.0f, 1.0f);
	DXSM::Vector4 worldPosFar = XMVector3TransformCoord(ndcPositionFar, invViewProj);

	rayDirection = worldPosFar - worldPos;
	rayDirection.Normalize();

	// auto trComp = m_scene.gameObjects[1]->GetComponent<TransformComponent>();
	// trComp->m_position = DXSM::Vector3(worldPos);
}

Sunshine::UUID WorldEditor::ChooseObjectByClick(UINT x, UINT y)
{
	return Sunshine::UUID(m_pixelUUIDHandler->GetUUID(m_renderer->GetDeviceContext(),
		m_gPass->pGBuffer->pUUIDSRV.Get(), x, y));
}
