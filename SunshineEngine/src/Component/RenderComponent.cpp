#include <Component/RenderComponent.h>
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

#include <Graphics/Renderer/DeferredRenderer.h>

void RenderComponent::AddTechnique(eastl::unique_ptr<SE_G::RenderTechnique> renderTech) {
	m_renderSystem->AddTechnique(eastl::move(renderTech));
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
