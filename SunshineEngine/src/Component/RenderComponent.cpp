#include "Component/RenderComponent.h"

void RenderComponent::DrawTechnique(eastl::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const noexcept
{
	 techniques.at(technique)->DrawTechnique(context);
}
bool RenderComponent::HasTechnique(eastl::string technique)
{
	return techniques.find(technique) != techniques.end();
}
void RenderComponent::PassTechnique(eastl::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	techniques[technique]->BindAll(context);
}