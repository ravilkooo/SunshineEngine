#include "Component/RenderComponent.h"

bool RenderComponent::HasTechnique(eastl::string technique)
{
	return techniques.find(technique) != techniques.end();
}

void RenderComponent::PassTechnique(eastl::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	techniques[technique]->Pass(context);
}
