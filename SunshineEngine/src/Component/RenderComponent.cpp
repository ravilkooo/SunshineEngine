#include <Component/RenderComponent.h>
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>
#include <Graphics/Renderer/Technique/GPassTechnique.h>

#include <Graphics/Renderer/DeferredRenderer.h>

SE_G::RenderTechnique* RenderComponent::AddTechnique(eastl::unique_ptr<SE_G::RenderTechnique> renderTech) {
	return m_renderSystem->AddTechnique(eastl::move(renderTech));
}

RenderComponent_Info::~RenderComponent_Info() {
	techniques.clear();
}

bool RenderComponent_Info::HasGPassMesh() {
	return m_hasGPassMesh;
}
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
