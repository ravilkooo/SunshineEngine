#include <Component/RenderComponent.h>
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>
#include <Graphics/Renderer/Technique/RenderTechnique.h>
#include <Graphics/Renderer/Technique/GPassTechnique.h>
#include <Graphics/Renderer/Technique/TransparentTechnique.h>

#include <Graphics/Renderer/DeferredRenderer.h>

RenderComponent::~RenderComponent() {
	m_renderSystem->RemoveAllTechniques(m_objectUUID);
}


SE_G::RenderTechnique* RenderComponent::AddTechnique(eastl::unique_ptr<SE_G::RenderTechnique> renderTech)
{
	return m_renderSystem->AddTechnique(m_objectUUID, eastl::move(renderTech));
}

SE_G::RenderTechnique* RenderComponent::GetTechnique(eastl::string technique)
{
	return m_renderSystem->GetTechnique(m_objectUUID, technique);
}

void RenderComponent::RemoveTechnique(eastl::string technique)
{
	m_renderSystem->RemoveTechnique(m_objectUUID, technique);
}

ID3D11Device* RenderComponent::GetDevice() { return m_renderSystem->GetDevice(); }
ID3D11DeviceContext* RenderComponent::GetDeviceContext() { return m_renderSystem->GetDeviceContext(); }

void RenderComponent::ApplyVisibility()
{
	if (!m_isVisible)
		m_renderSystem->DisableAllTechniques(m_objectUUID);
}

bool RenderComponent::GetVisibility()
{
	return m_isVisible;
}

void RenderComponent::SetVisibility(bool newVisibiilty)
{
	if (m_isVisible && !newVisibiilty)
		m_renderSystem->DisableAllTechniques(m_objectUUID);
	else if (!m_isVisible && newVisibiilty)
		m_renderSystem->EnableAllTechniques(m_objectUUID);

	m_isVisible = newVisibiilty;
}

void RenderComponent::ToggleVisibility()
{
	m_isVisible = !m_isVisible;

	if (!m_isVisible)
		m_renderSystem->DisableAllTechniques(m_objectUUID);
	else
		m_renderSystem->EnableAllTechniques(m_objectUUID);
}

RenderComponent_Info::RenderComponent_Info(SE::UUID uuid, SE_G::DeferredRenderer* renderSystem)
{
	m_assignedComponent = eastl::make_unique<RenderComponent>(uuid, renderSystem);
}

RenderComponent_Info::~RenderComponent_Info() {
	eastl::unordered_set techs_copy = techniques;
	for (auto tech : techs_copy)
	{
		RemoveTechnique(tech);
	}
	techniques.clear();
}

bool RenderComponent_Info::HasGPassMesh() {
	return m_hasGPassMesh;
}

void RenderComponent_Info::AddTechnique_Info(SE_G::RenderTechnique* tech)
{
	if (tech->GetTechniqueTag() == "IconPass") {
		m_selectionTechnique = tech;
	}
	else if (tech->GetTechniqueTag() == "GPass") {
		m_selectionTechnique = tech;
		m_hasGPassMesh = true;
		m_gPassTech = static_cast<SE_G::GPassTechnique*>(tech);
		m_gPassTech->m_isHiddenInEditor = !m_isVisible;
	}
	else if (tech->GetTechniqueTag() == "Transparent") {
		m_selectionTechnique = tech;
		m_hasGPassMesh = true;
		m_transparentTech = static_cast<SE_G::TransparentTechnique*>(tech);
		m_transparentTech->m_isHiddenInEditor = !m_isVisible;
	}

	techniques.insert(tech->GetTechniqueTag());
}

SE_G::RenderTechnique* RenderComponent_Info::AddTechnique(eastl::unique_ptr<SE_G::RenderTechnique> tech)
{
	AddTechnique_Info(tech.get());
	return m_assignedComponent->AddTechnique(eastl::move(tech));
}

SE_G::RenderTechnique* RenderComponent_Info::GetTechnique(eastl::string technique)
{
	auto res = techniques.find(technique);
	if (res != techniques.end())
	{
		auto tech = m_assignedComponent->GetTechnique(technique);
		return tech;
	}
	else
		return nullptr;
}

bool RenderComponent_Info::HasTechnique(eastl::string technique) {
	return (techniques.find(technique) != techniques.end());
}

void RenderComponent_Info::RemoveTechnique(eastl::string technique) {
	if (techniques.find(technique) != techniques.end())
	{
		if (technique == "GPass")
		{
			m_selectionTechnique = nullptr;
			if (HasTechnique("IconPass"))
			{
				m_selectionTechnique = m_assignedComponent->GetTechnique("IconPass");
			}
		}
		else if (technique == "Transparent")
		{
			m_selectionTechnique = nullptr;
			if (HasTechnique("IconPass"))
			{
				m_selectionTechnique = m_assignedComponent->GetTechnique("IconPass");
			}
		}

		techniques.erase(technique);
		m_assignedComponent->RemoveTechnique(technique);
	}
}

void RenderComponent_Info::ApplyVisibility()
{
	if (m_gPassTech)
		m_gPassTech->m_isHiddenInEditor = !m_isVisible;
	else if (m_transparentTech)
		m_transparentTech->m_isHiddenInEditor = !m_isVisible;
}

bool RenderComponent_Info::GetVisibility()
{
	return m_isVisible;
}

void RenderComponent_Info::SetVisibility(bool newVisibilty)
{
	m_isVisible = newVisibilty;
	if (m_gPassTech)
		m_gPassTech->m_isHiddenInEditor = !m_isVisible;
	else if (m_transparentTech)
		m_transparentTech->m_isHiddenInEditor = !m_isVisible;
}

void RenderComponent_Info::ToggleVisibility()
{
	m_isVisible = !m_isVisible;
	if (m_gPassTech)
		m_gPassTech->m_isHiddenInEditor = !m_isVisible;
	else if (m_transparentTech)
		m_transparentTech->m_isHiddenInEditor = !m_isVisible;
}