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
		GPass* gPass = new GPass(
			m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->GetBackBuffer(),
			m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(gPass);
	}
	{
		LightPass* lightPass = new LightPass(
			m_renderer->GetDevice(), m_renderer->GetDeviceContext(),
			m_renderer->GetBackBuffer(),
			m_renderer->pGBuffer, m_renderer->GetMainCamera());

		m_renderer->AddPass(lightPass);
	}

	/*
	TestObjects

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
	*/
}
void WorldEditor::Run() {
	
}

void WorldEditor::Update(float deltaTime) {
	
}

void WorldEditor::Render() {
	
}