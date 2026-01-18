#include <Component/RenderComponent.h>
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>
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

/*
void RenderComponent_Info::SetMesh(const eastl::string& filePath) {
	m_gPassTech->SetMesh(filePath);
}

void RenderComponent_Info::SetMesh(eastl::shared_ptr<SE_G::Mesh> newMesh) {
	m_gPassTech->SetMesh(newMesh);
}

void RenderComponent_Info::SetMeshTexture(const eastl::wstring& filePath,
	SE_G::Bind::SamplerPreset samplerPreset) {
	m_gPassTech->SetTexture(filePath, samplerPreset);
}

eastl::string RenderComponent_Info::GetCurrentMeshPath() const {
	return m_gPassTech->m_mesh->GetCurrentMeshPath();
}

eastl::wstring RenderComponent_Info::GetCurrentTexturePath() const {
	return m_gPassTech->m_texture->GetCurrentTexturePath();
}

SE_G::Bind::SamplerPreset RenderComponent_Info::GetCurrentTextureSampler() const {
	return m_gPassTech->m_textureSampler->m_preset;
}
*/

/*
bool RenderComponent::HasTechnique(eastl::string technique)
{
	return techniques.find(technique) != techniques.end();
}

void RenderComponent::PassTechnique(eastl::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	techniques[technique]->Pass(context);
}
*/


// #define RC_ADD_METHOD(k, fn) k, fn
// LUA_REGISTER_COMPONENT(
//     RenderComponent,
//     "RenderComponent",
//     /* no fields */ ,
//     RENDERCOMPONENT_LUA_METHODS_APPLY(RC_ADD_METHOD),
//     "getRender")
// #undef RC_ADD_METHOD
