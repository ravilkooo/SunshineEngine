#include <Component/RenderComponent.h>
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>
#include <Graphics/Renderer/Technique/RenderTechnique.h>
#include <Graphics/Renderer/Technique/GPassTechnique.h>

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
	}

	techniques.insert(tech->GetTechniqueTag());
}

SE_G::RenderTechnique* RenderComponent_Info::AddTechnique(eastl::unique_ptr<SE_G::RenderTechnique> tech)
{
	AddTechnique_Info(tech.get());
	return m_assignedComponent->AddTechnique(eastl::move(tech));
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

		techniques.erase(technique);
		m_assignedComponent->RemoveTechnique(technique);
	}
}

