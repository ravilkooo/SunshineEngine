#include "WorldEditor.h"


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

	/*
	TestObjects
	*/

	GameObjectFactory factory;

	m_scene.AddGameObject(factory.CreateSkyBox(
		m_renderer->GetDevice(),
		m_renderer->GetMainCamera()));
	m_scene.AddGameObject(factory.CreateDefaultBoxObject(
		m_renderer->GetDevice()));
	m_scene.AddGameObject(factory.CreateAmbientLightObject(
		m_renderer->GetDevice(),
		m_renderer->GetMainCamera(),
		{ DXSM::Vector3::One, 1.0f }
		));
}
void WorldEditor::Run() {
	
}

void WorldEditor::Update(float deltaTime) {
	m_scene.gameObjects[1]->GetComponent<TransformComponent>()->m_localRotation.y += deltaTime;
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
}