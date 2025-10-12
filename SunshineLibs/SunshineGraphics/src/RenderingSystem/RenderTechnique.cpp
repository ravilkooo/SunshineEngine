

#include "RenderingSystem/RenderTechnique.h"
#include "Bindable/Bindable.h"

using namespace Bind;

RenderTechnique::RenderTechnique(std::string technique)
	: techniqueTag(technique)
{
}

void RenderTechnique::AddBind(Bind::Bindable* bind)
{
	bindables.push_back(bind);
}

void RenderTechnique::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	for (size_t i = 0; i < bindables.size(); i++)
	{
		bindables[i]->Bind(context.Get());
	}

	if (vertexShader)
		vertexShader->Bind(context.Get());

	if (pixelShader)
		pixelShader->Bind(context.Get());
	
	if (texture) {
		texture->Bind(context.Get());
	}

	if (textureSampler) {
		textureSampler->Bind(context.Get());
	}
}

void RenderTechnique::DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	if (mesh)
		mesh->Draw(context.Get());

}

std::string RenderTechnique::GetTechnique()
{
	return techniqueTag;
}
